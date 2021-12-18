#pragma once //"exitCodeEnum.hpp"

#include <iostream>
#include <fstream>
#include <archive.h>
#include <archive_entry.h>
#include <vector>
#include "exitCodeEnum.hpp"
#include "Exceptions/GzipWriteReadException.h"
#include <config.h>

namespace compress {
    const int bufferSize = 1024 * 1024 * 4;

    void write_archive(const std::string &rootPath, const char *outname, std::vector<std::string> files) {
        struct archive *archive;
        struct archive_entry *archiveEntry;
        struct stat st;
        int len;
        std::ifstream fileStream;
        char buffer[bufferSize];

        archive = archive_write_new();
        if (
                archive_write_add_filter_gzip(archive) ||
                archive_write_set_format_pax_restricted(archive) ||
                archive_write_open_filename(archive, outname) != 0)
            throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));

        for (auto iterator = files.begin(); iterator != files.end(); iterator++) {
            std::string fileName = *iterator;

            lstat(fileName.c_str(), &st);

            archiveEntry = archive_entry_new();

            archive_entry_set_pathname_utf8(archiveEntry, fileName.c_str());
            archive_entry_copy_stat(archiveEntry, &st);
            if (S_ISLNK(st.st_mode)) {
                archive_entry_set_filetype(archiveEntry, AE_IFLNK);
                archive_entry_set_symlink(archiveEntry, stdfs::read_symlink(fileName).c_str());
            }

            if (archive_write_header(archive, archiveEntry) != 0)
                throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));

            if (!S_ISLNK(st.st_mode)) {

                fileStream.open(fileName, std::ifstream::binary | std::ifstream::out);

                while (fileStream.good()) {
                    fileStream.read(buffer, sizeof(buffer));
                    archive_write_data(archive, buffer, (size_t) fileStream.gcount());
                }

                fileStream.close();
            }

            archive_entry_free(archiveEntry);

        }

        if (
                archive_write_close(archive) ||
                archive_write_free(archive) != 0)
            throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));
    }

    static int copy_data(struct archive *ar, struct archive *aw) {
        int r;
        const void *buff;
        size_t size;
        off_t offset;

        for (;;) {
            r = archive_read_data_block(ar, &buff, &size, &offset);
            if (r == ARCHIVE_EOF)
                return (ARCHIVE_OK);
            if (r < ARCHIVE_OK)
                return (r);
            r = archive_write_data_block(aw, buff, size, offset);
            if (r < ARCHIVE_OK) {
                fprintf(stderr, "%s\n", archive_error_string(aw));
                return (r);
            }
        }
    }

    static int extract(const char *filename) {
        struct archive *a;
        struct archive *ext;
        struct archive_entry *entry;
        int flags;
        int r;

        /* Select which attributes we want to restore. */
        flags = ARCHIVE_EXTRACT_TIME;
        flags |= ARCHIVE_EXTRACT_PERM;
        flags |= ARCHIVE_EXTRACT_ACL;
        flags |= ARCHIVE_EXTRACT_FFLAGS;

        a = archive_read_new();

        if (archive_read_support_filter_all(a) ||
            archive_read_support_format_all(a) != 0)
            throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));

        ext = archive_write_disk_new();
        if (archive_write_disk_set_options(ext, flags) ||
            archive_write_disk_set_standard_lookup(ext) != 0)
            throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));

        if ((r = archive_read_open_filename(a, filename, 10240))) {
            fprintf(stderr, "%s\n", archive_error_string(a));

            return 1;
        }

        for (;;) {
            r = archive_read_next_header(a, &entry);
            if (r == ARCHIVE_EOF)
                break;
            if (r < ARCHIVE_OK)
                throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));
            if (r < ARCHIVE_WARN)
                throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));
            r = archive_write_header(ext, entry);
            if (r < ARCHIVE_OK)
                throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));
            else if (archive_entry_size(entry) > 0) {
                r = copy_data(a, ext);
                if (r < ARCHIVE_OK) {
                    throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));
                }
                if (r < ARCHIVE_WARN)
                    throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));
            }
            r = archive_write_finish_entry(ext);
            if (r < ARCHIVE_OK)
                throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));
            if (r < ARCHIVE_WARN)
                throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));
        }

        if (archive_read_close(a) ||
            archive_read_free(a) ||
            archive_write_close(ext) ||
            archive_write_free(ext) != 0)
            throw (GzipWriteReadException("GZip Exception", ExitCode::gzipException));

        return 0;
    }
}