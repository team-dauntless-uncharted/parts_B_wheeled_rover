#pragma once
#include <Arduino.h>
#include <PosixAvi.hpp>
#include "Utils/FileIO/BaseFileIO.hpp"

class SDLogger : public BaseFileIO {
public:
    SDLogger();

    bool begin(const String &csvHeader);

    bool appendSystemLog(const char* message);
    bool appendSensorLog(const char* message);
    bool appendSensorLog(const char* message, size_t length);

    bool saveJPEGImage(void* buff, size_t size);
    bool savePPMImage(void* buff, size_t size);

    bool aviInit(int width, int height);  // 変更: voidからboolに変更
    void aviStart();
    void aviRecord(void* buff, size_t size);
    void aviEnd();

    bool readState(int &state);
    bool writeState(const int &state);

    size_t readJSONFile(const char* filename, char* buffer, size_t bufferSize);

protected:
    const char *getBasePath() const override;

private:
    bool waitForSDMount(int timeout_ms = 5000);

    void refreshFileNameIndex(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter);
    void shiftFileName(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter);

    // SystemLog
    char _systemLogFileName[32];
    uint16_t _systemLogFileNameCount = 0;

    void refreshSystemLogFileNameIndex();
    void shiftSystemLogFileName();

    // SensorLog
    char _sensorLogFileName[32];
    uint16_t _sensorLogFileNameCount = 0;

    void refreshSensorLogFileNameIndex();
    void shiftSensorLogFileName();

    bool appendLog(const char* filename, const char* message);

    // JPEG
    char _jpegFileName[32];
    uint16_t _jpegFileNameCount = 0;

    void refreshJPEGFileNameIndex();
    void shiftJPEGFileName();

    // PPM
    char _ppmFileName[32];
    uint16_t _ppmFileNameCount = 0;

    void refreshPPMFileNameIndex();
    void shiftPPMFileName();

    // AVI
    char _aviFileName[32];
    uint16_t _aviFileNameCount = 0;

    void refreshAVIFileNameIndex();
    void shiftAVIFileName();

    PosixAviLibrary _avi;  // 変更: AviLibrary から PosixAviLibrary に変更
};