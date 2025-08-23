#include "Logger.hpp"

Logger::Logger() : _aviFd(-1) {}

// ------------------ POSIXラッパー ------------------

bool Logger::posixOpen(const char* filename, bool write, int &fd) {
    if (write) {
        fd = ::open(filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
    } else {
        fd = ::open(filename, O_RDONLY);
    }
    return fd >= 0;
}

void Logger::posixClose(int &fd) {
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }
}

ssize_t Logger::posixWrite(int fd, const void* buf, size_t size) {
    return ::write(fd, buf, size);
}

ssize_t Logger::posixRead(int fd, void* buf, size_t size) {
    return ::read(fd, buf, size);
}

// ------------------ 初期化 ------------------
bool Logger::begin(const String &csvHeader) {
    if (!waitForSDMount()) {
        return false;
    }
    refreshSystemLogFileNameIndex();
    refreshSensorLogFileNameIndex();
    refreshJPEGFileNameIndex();
    refreshPPMFileNameIndex();

    return appendSensorLog(csvHeader.c_str());
}

bool Logger::waitForSDMount(int timeout_ms = 5000) {
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

bool Logger::appendLog(const char* filename, const char* message) {
    int fd;
    if (!posixOpen(filename, true, fd)) return false;

    ssize_t written = posixWrite(fd, message, strlen(message));
    posixWrite(fd, "\n", 1);

    posixClose(fd);
    return written == (ssize_t)strlen(message);
}

bool Logger::appendSystemLog(const char* message) {
    return appendLog(_systemLogFileName, message);
}

bool Logger::appendSensorLog(const char* message) {
    return appendLog(_sensorLogFileName, message);
}

bool Logger::appendSensorLog(const char* message, size_t length) {
    int fd;
    if (!posixOpen(_sensorLogFileName, true, fd)) return false;

    ssize_t written = posixWrite(fd, message, length);
    posixClose(fd);

    return written == (ssize_t)length;
}

// ------------------ JPEG / PPM ------------------

bool Logger::saveJPEGImage(void* buff, size_t size) {
    int fd;
    if (!posixOpen(_jpegFileName, true, fd)) return false;

    posixWrite(fd, buff, size);
    posixClose(fd);

    shiftJPEGFileName();
    return true;
}

bool Logger::savePPMImage(void* buff, size_t size) {
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

void Logger::aviInit(int width, int height) {
    refreshAVIFileNameIndex();
    if (!posixOpen(_aviFileName, true, _aviFd)) return;
    _avi.begin(_aviFd, width, height);
}

void Logger::aviStart() {
    _avi.startRecording();
}

void Logger::aviRecord(void* buff, size_t size) {
    _avi.addFrame(buff, size);
}

void Logger::aviEnd() {
    _avi.endRecording();
    _avi.end();
    posixClose(_aviFd);
}

// ------------------ State ------------------

bool Logger::readState(int &state) {
    int fd;
    if (!posixOpen("/mnt/sd0/state.txt", false, fd)) return false;

    char buf[32] = {};
    ssize_t n = posixRead(fd, buf, sizeof(buf) - 1);
    posixClose(fd);

    if (n <= 0) return false;
    state = atoi(buf);
    return true;
}

bool Logger::writeState(const int &state) {
    int fd;
    if (!posixOpen("/mnt/sd0/state.txt", true, fd)) return false;

    char buf[16];
    int n = snprintf(buf, sizeof(buf), "%d", state);
    posixWrite(fd, buf, n);
    posixClose(fd);
    return true;
}

// ------------------ ファイル名管理 ------------------

void Logger::refreshFileNameIndex(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter) {
    while (true) {
        shiftFileName(fileNameBuf, bufSize, format, counter);
        struct stat st;
        if (::stat(fileNameBuf, &st) != 0) break; // ファイルが存在しなければ終了
    }
}

void Logger::shiftFileName(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter) {
    snprintf(fileNameBuf, bufSize, format, counter++);
}

void Logger::refreshSystemLogFileNameIndex() { refreshFileNameIndex(_systemLogFileName, sizeof(_systemLogFileName), "/mnt/sd0/system_%03d.log", _systemLogFileNameCount); }
void Logger::shiftSystemLogFileName() { shiftFileName(_systemLogFileName, sizeof(_systemLogFileName), "/mnt/sd0/system_%03d.log", _systemLogFileNameCount); }

void Logger::refreshSensorLogFileNameIndex() { refreshFileNameIndex(_sensorLogFileName, sizeof(_sensorLogFileName), "/mnt/sd0/sensor_%03d.csv", _sensorLogFileNameCount); }
void Logger::shiftSensorLogFileName() { shiftFileName(_sensorLogFileName, sizeof(_sensorLogFileName), "/mnt/sd0/sensor_%03d.csv", _sensorLogFileNameCount); }

void Logger::refreshJPEGFileNameIndex() { refreshFileNameIndex(_jpegFileName, sizeof(_jpegFileName), "/mnt/sd0/photo_%03d.jpg", _jpegFileNameCount); }
void Logger::shiftJPEGFileName() { shiftFileName(_jpegFileName, sizeof(_jpegFileName), "/mnt/sd0/photo_%03d.jpg", _jpegFileNameCount); }

void Logger::refreshPPMFileNameIndex() { refreshFileNameIndex(_ppmFileName, sizeof(_ppmFileName), "/mnt/sd0/detection_%03d.ppm", _ppmFileNameCount); }
void Logger::shiftPPMFileName() { shiftFileName(_ppmFileName, sizeof(_ppmFileName), "/mnt/sd0/detection_%03d.ppm", _ppmFileNameCount); }

void Logger::refreshAVIFileNameIndex() { refreshFileNameIndex(_aviFileName, sizeof(_aviFileName), "/mnt/sd0/video_%03d.avi", _aviFileNameCount); }
void Logger::shiftAVIFileName() { shiftFileName(_aviFileName, sizeof(_aviFileName), "/mnt/sd0/video_%03d.avi", _aviFileNameCount); }