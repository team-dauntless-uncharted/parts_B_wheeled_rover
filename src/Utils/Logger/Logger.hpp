#pragma once
#include <SDHCI.h>
#include <File.h>

#include <AviLibrary.h>

class Logger {
public:
    Logger();
    // 初期化
    bool begin(String csvHeader);

    // ログの追加
    bool appendLog(const char* message);

    // JPEGファイルの保存
    bool saveJPEGImage(void* buff, size_t size);

    // PPMファイルの保存
    bool savePPMImage(void* buff, size_t size);

    // AVI
    void aviInit(int width, int height);
    void aviStart();
    // AVI動画撮影
    void aviRecord(void* buff, size_t size);
    // AVI撮影終了
    void aviEnd();

private:
    SDClass _sd;

    AviLibrary _avi;
    File _aviFile;

    // log
    char _logFileName[32];
    uint16_t _logFileNameCount = 0;

    // JPEGファイル
    char _jpegFileName[32];
    uint16_t _jpegFileNameCount = 0;

    // PPMファイル
    char _ppmFileName[32];
    uint16_t _ppmFileNameCount = 0;
    
    bool sdInit();
    bool createLogFile(String header);

    void refreshFileNameIndex(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter);
    void shiftFileName(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter);

    // _jpegFileNameCountの開始番号を決める
    void refreshJPEGFileNameIndex();
    // JPEGファイルのインクリメント
    void shiftJPEGFileName();

    // _ppmFileNameCountの開始番号を決める
    void refreshPPMFileNameIndex();
    // PPMファイルのインクリメント
    void shiftPPMFileName();
};