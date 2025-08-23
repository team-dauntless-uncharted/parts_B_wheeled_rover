#pragma once
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#include <AviLibrary.h>

class Logger {
public:
    Logger();

    bool begin(const String &csvHeader);

    bool appendSystemLog(const char* message);
    bool appendSensorLog(const char* message);
    bool appendSensorLog(const char* message, size_t length);

    bool saveJPEGImage(void* buff, size_t size);
    bool savePPMImage(void* buff, size_t size);

    void aviInit(int width, int height);
    void aviStart();
    void aviRecord(void* buff, size_t size);
    void aviEnd();

    bool readState(int &state);
    bool writeState(const int &state);

private:
    bool waitForSDMount(int timeout_ms = 5000);

    // POSIX ファイル操作
    bool posixOpen(const char* filename, bool write, int &fd);
    void posixClose(int &fd);
    ssize_t posixWrite(int fd, const void* buf, size_t size);
    ssize_t posixRead(int fd, void* buf, size_t size);

    void refreshFileNameIndex(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter);
    void shiftFileName(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter);

    // SystemLog
    char _systemLogFileName[24];
    uint16_t _systemLogFileNameCount = 0;

    void refreshSystemLogFileNameIndex();
    void shiftSystemLogFileName();

    // SensorLog
    char _sensorLogFileName[24];
    uint16_t _sensorLogFileNameCount = 0;

    void refreshSensorLogFileNameIndex();
    void shiftSensorLogFileName();

    bool appendLog(const char* filename, const char* message);

    // JPEG
    char _jpegFileName[24];
    uint16_t _jpegFileNameCount = 0;

    void refreshJPEGFileNameIndex();
    void shiftJPEGFileName();

    // PPM
    char _ppmFileName[24];
    uint16_t _ppmFileNameCount = 0;

    void refreshPPMFileNameIndex();
    void shiftPPMFileName();

    // AVI
    char _aviFileName[24];
    uint16_t _aviFileNameCount = 0;

    void refreshAVIFileNameIndex();
    void shiftAVIFileName();

    AviLibrary _avi;
    int _aviFd;
};