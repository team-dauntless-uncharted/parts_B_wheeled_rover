#pragma once
#include <SDHCI.h>
#include <File.h>

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

private:
    SDClass _sd;
    File _myFile;

    // JPEGファイル
    char _jpegFileName[32];
    uint16_t _jpegFileNameCount = 0;

    // PPMファイル
    char _ppmFileName[32];
    uint16_t _ppmFileNameCount = 0;
    
    bool sdInit();
    bool createLogFile(String header);

    // _jpegFileNameCountの開始番号を決める
    void refreshJPEGFileNameIndex();
    // JPEGファイルのインクリメント
    void shiftJPEGFileName();

    // _ppmFileNameCountの開始番号を決める
    void refreshPPMFileNameIndex();
    // PPMファイルのインクリメント
    void shiftPPMFileName();
};