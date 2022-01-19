enum ExitCode {
    ok = 0,
    argumentParsingFailed = 8,
    identityFileFailed = 9,
    setupCommandFailed = 10,
    finalizeCommandFailed = 11,
    copyToCacheFailed = 12,
    copyFromCacheFailed = 13,
    createSymLinkFailed = 14,
    cleaningFailed = 15,
    createCacheDirectoriesFailed = 16,
    gzipException = 17,
};