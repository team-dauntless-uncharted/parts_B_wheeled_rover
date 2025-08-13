#pragma once
#include <SDHCI.h>
#include <File.h>

class Logger {
public:
    Logger();
    // 初期化
    bool begin();

    // ログの追加
    bool appendLog(const char* message);

    // ログの作成
    const char* createMessage(unsigned long currentTime, const String& currentDate, 
                        int state, double lat, double lng, double alt,
                        double distance, double direction, int mr_pwm, int ml_pwm,
                        int mOutputTime, int cds, double ax, double ay, double az,
                        double gx, double gy, double gz, double mx, double my, double mz,
                        double roll, double pitch, double heading);

    const char* createMessage(const String& currentDate, int state,
                              double lat, double lng, double alt,
                              int mr_pwm, int ml_pwm);

    // JPEGファイルの保存
    bool saveJPEGImage(void* buff, size_t size);

    // PPMファイルの保存
    bool savePPMImage(void* buff, size_t size);

private:
    SDClass _sd;
    File _myFile;

    // CSVファイル
    const String CSV_HEADER = "time,date,mode,lat,lng,alt,distance,direction,mr_pwm,ml_pwm,mOutputTime,cds,ax,ay,az,gx,gy,gz,mx,my,mz,roll,pitch,heading";
    char _logBuffer[300];

    // JPEGファイル
    char _jpegFileName[32];
    uint16_t _jpegFileNameCount = 0;

    // PPMファイル
    char _ppmFileName[32];
    uint16_t _ppmFileNameCount = 0;
    
    bool sdInit();
    bool createLogFile();

    // _jpegFileNameCountの開始番号を決める
    void refreshJPEGFileNameIndex();
    // JPEGファイルのインクリメント
    void shiftJPEGFileName();

    // _ppmFileNameCountの開始番号を決める
    void refreshPPMFileNameIndex();
    // PPMファイルのインクリメント
    void shiftPPMFileName();

    // Tweliteでのデータ送信
    void tweliteSend(const char* message);
};