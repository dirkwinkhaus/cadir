enum ExitCode {
    ok = 0,
    createdCache = 1,
    argumentParsingFailed = 20,
    identityFileFailed = 30,
    setupCommandFailed = 40,
    finalizeCommandFailed = 50,
    copyToCacheFailed = 60,
    copyFromCacheFailed = 70,
    createSymLinkFailed = 80,
    cleaningFailed = 90,
    createCacheDirectoriesFailed = 100,
    gzipException = 110,
};