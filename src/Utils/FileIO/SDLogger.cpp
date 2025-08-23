#include "SDLogger.hpp"

SDLogger::SDLogger() {}

const char *SDLogger::getBasePath() const {
    return "/mnt/sd0";
}

// ------------------ 初期化 ------------------
bool SDLogger::begin(const String &csvHeader) {
    if (!waitForSDMount()) {
        return false;
    }
    refreshSystemLogFileNameIndex();
    refreshSensorLogFileNameIndex();
    refreshJPEGFileNameIndex();
    refreshPPMFileNameIndex();

    return appendSensorLog(csvHeader.c_str());
}

bool SDLogger::waitForSDMount(int timeout_ms = 5000) {
    struct stat st;
    int elapsed = 0;
    const int interval = 100;
    while (stat("/mnt/sd0", &st) != 0) {
        delay(interval);
        elapsed += interval;
        if (elapsed >= timeout_ms) return false;
    }
    return true;
}

// ------------------ ログ書き込み ------------------

bool SDLogger::appendLog(const char* filename, const char* message) {
    int fd;
    if (!posixOpen(filename, true, fd)) return false;

    ssize_t written = posixWrite(fd, message, strlen(message));
    posixWrite(fd, "\n", 1);

    posixClose(fd);
    return written == (ssize_t)strlen(message);
}

bool SDLogger::appendSystemLog(const char* message) {
    return appendLog(_systemLogFileName, message);
}

bool SDLogger::appendSensorLog(const char* message) {
    return appendLog(_sensorLogFileName, message);
}

bool SDLogger::appendSensorLog(const char* message, size_t length) {
    int fd;
    if (!posixOpen(_sensorLogFileName, true, fd)) return false;

    ssize_t written = posixWrite(fd, message, length);
    posixClose(fd);

    return written == (ssize_t)length;
}

// ------------------ JPEG / PPM ------------------

bool SDLogger::saveJPEGImage(void* buff, size_t size) {
    int fd;
    if (!posixOpen(_jpegFileName, true, fd)) return false;

    posixWrite(fd, buff, size);
    posixClose(fd);

    shiftJPEGFileName();
    return true;
}

bool SDLogger::savePPMImage(void* buff, size_t size) {
    int fd;
    if (!posixOpen(_ppmFileName, true, fd)) return false;

    char header[64];
    int hlen = snprintf(header, sizeof(header), "P6\n%lu %lu\n255\n", 96, 96);
    posixWrite(fd, header, hlen);
    posixWrite(fd, buff, size);
    posixClose(fd);

    shiftPPMFileName();
    return true;
}

// ------------------ AVI ------------------

bool SDLogger::aviInit(int width, int height) {  // 戻り値をboolに変更
    refreshAVIFileNameIndex();
    
    // POSIX API対応版は内部でファイルを管理するため、posixOpenは不要
    return _avi.begin(_aviFileName, width, height);  // 戻り値をチェック
}

void SDLogger::aviStart() {
    _avi.startRecording();
}

void SDLogger::aviRecord(void* buff, size_t size) {
    // 型変換を追加（PosixAviLibraryはconst char*を要求）
    _avi.addFrame(static_cast<const char*>(buff), size);
}

void SDLogger::aviEnd() {
    _avi.endRecording();
    _avi.end();
    // posixClose(_aviFd); を削除（PosixAviLibraryが内部で管理）
    
    // 次のファイル名に更新
    shiftAVIFileName();
}

// ------------------ State ------------------

bool SDLogger::readState(int &state) {
    return BaseFileIO::readState("/mnt/sd0/state.txt", state);
}

bool SDLogger::writeState(const int &state) {
    return BaseFileIO::writeState("/mnt/sd0/state.txt", state);
}

// ------------------ ファイル名管理 ------------------

void SDLogger::refreshFileNameIndex(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter) {
    while (true) {
        shiftFileName(fileNameBuf, bufSize, format, counter);
        struct stat st;
        if (::stat(fileNameBuf, &st) != 0) break; // ファイルが存在しなければ終了
    }
}

void SDLogger::shiftFileName(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter) {
    snprintf(fileNameBuf, bufSize, format, counter++);
}

void SDLogger::refreshSystemLogFileNameIndex() { refreshFileNameIndex(_systemLogFileName, sizeof(_systemLogFileName), "/mnt/sd0/system_%03d.log", _systemLogFileNameCount); }
void SDLogger::shiftSystemLogFileName() { shiftFileName(_systemLogFileName, sizeof(_systemLogFileName), "/mnt/sd0/system_%03d.log", _systemLogFileNameCount); }

void SDLogger::refreshSensorLogFileNameIndex() { refreshFileNameIndex(_sensorLogFileName, sizeof(_sensorLogFileName), "/mnt/sd0/sensor_%03d.csv", _sensorLogFileNameCount); }
void SDLogger::shiftSensorLogFileName() { shiftFileName(_sensorLogFileName, sizeof(_sensorLogFileName), "/mnt/sd0/sensor_%03d.csv", _sensorLogFileNameCount); }

void SDLogger::refreshJPEGFileNameIndex() { refreshFileNameIndex(_jpegFileName, sizeof(_jpegFileName), "/mnt/sd0/photo_%03d.jpg", _jpegFileNameCount); }
void SDLogger::shiftJPEGFileName() { shiftFileName(_jpegFileName, sizeof(_jpegFileName), "/mnt/sd0/photo_%03d.jpg", _jpegFileNameCount); }

void SDLogger::refreshPPMFileNameIndex() { refreshFileNameIndex(_ppmFileName, sizeof(_ppmFileName), "/mnt/sd0/detection_%03d.ppm", _ppmFileNameCount); }
void SDLogger::shiftPPMFileName() { shiftFileName(_ppmFileName, sizeof(_ppmFileName), "/mnt/sd0/detection_%03d.ppm", _ppmFileNameCount); }

void SDLogger::refreshAVIFileNameIndex() { refreshFileNameIndex(_aviFileName, sizeof(_aviFileName), "/mnt/sd0/video_%03d.avi", _aviFileNameCount); }
void SDLogger::shiftAVIFileName() { shiftFileName(_aviFileName, sizeof(_aviFileName), "/mnt/sd0/video_%03d.avi", _aviFileNameCount); }