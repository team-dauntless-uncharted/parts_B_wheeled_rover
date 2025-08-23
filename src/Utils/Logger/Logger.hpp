#pragma once
#include <SDHCI.h>
#include <File.h>

#include <AviLibrary.h>

class Logger {
public:
    Logger();
    // 初期化
    bool begin(String csvHeader);

    bool appendSystemLog(const char* message);

    bool appendSensorLog(const char* message);
    bool appendSensorLog(const char* message, size_t length);

    // JPEGファイルの保存
    bool saveJPEGImage(void* buff, size_t size);

    // PPMファイルの保存
    bool savePPMImage(void* buff, size_t size);

    // AVI
    void aviInit(int width, int height);
    void aviStart();
    void aviRecord(void* buff, size_t size);
    void aviEnd();

    bool readState(int &state);
    bool writeState(const int &state);

private:
    SDClass _sd;

    AviLibrary _avi;
    File _aviFile;

    bool sdInit();

    bool appendLog(const char* filename, const char* message);

    void refreshFileNameIndex(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter);
    void shiftFileName(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter);

    /**
     * SystemLog
     */
    char _systemLogFileName[24];
    uint16_t _systemLogFileNameCount = 0;

    void refreshSystemLogFileNameIndex();
    void shiftSystemLogFileName();

    /**
     * SensorLog
     */
    char _sensorLogFileName[24];
    uint16_t _sensorLogFileNameCount = 0;

    void refreshSensorLogFileNameIndex();
    void shiftSensorLogFileName();

    /**
     * .jpg
     */
    char _jpegFileName[24];
    uint16_t _jpegFileNameCount = 0;

    void refreshJPEGFileNameIndex();
    void shiftJPEGFileName();

    /**
     * .ppm 
     */
    char _ppmFileName[24];
    uint16_t _ppmFileNameCount = 0;

    void refreshPPMFileNameIndex();
    void shiftPPMFileName();

    /**
     * .avi
     */
    char _aviFileName[24];
    uint16_t _aviFileNameCount = 0;

    void refreshAVIFileNameIndex();
    void shiftAVIFileName();
};