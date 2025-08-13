#include "Logger.hpp"
#include <Arduino.h>

#define TweliteSend(c) Serial2.println(c)

Logger::Logger() : _sd(), _myFile() {}

// 初期化
bool Logger::begin() {
    if (!sdInit()) {
        return false;
    }
    
    if (!createLogFile()) {
        return false;
    }

    refreshJPEGFileNameIndex();
    refreshPPMFileNameIndex();
    
    return true;
}

// ログの追加
bool Logger::appendLog(const char* message) {
    _myFile = _sd.open("log/log.csv", FILE_WRITE);
    
    if (!_myFile) {
        return false;
    }
    
    _myFile.println(message);
    _myFile.close();
    
    Serial.println(message);
    
    return true;
}

// ログの作成
const char* Logger::createMessage(unsigned long currentTime, const String& currentDate, 
                           double lat, double lng, double alt,
                           double distance, double direction, int mr_pwm, int ml_pwm,
                           int mOutputTime, int cds, double ax, double ay, double az,
                           double gx, double gy, double gz, double mx, double my, double mz,
                           double roll, double pitch, double heading) {
    // snprintfでフォーマットされた文字列を生成
    // 注意: AVRベースのArduinoでは、浮動小数点数のサポートに特別な設定が必要な場合があります
    snprintf(_logBuffer, sizeof(_logBuffer),
             "%lu,%s,%.6f,%.6f,%.2f,%.2f,%.2f,%d,%d,%d,%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.2f,%.2f,%.2f",
             currentTime,
             currentDate.c_str(),
             lat, lng, alt,
             distance, direction,
             mr_pwm, ml_pwm,
             mOutputTime, cds,
             ax, ay, az,
             gx, gy, gz,
             mx, my, mz,
             roll, pitch, heading);

    return _logBuffer;
}

const char* Logger::createMessage(const String& currentDate,
                                  double lat, double lng, double alt,
                                  int mr_pwm, int ml_pwm) {
    snprintf(_logBuffer, sizeof(_logBuffer),
             "%s,%.6f,%.6f,%.2f,%d,%d",
             currentDate.c_str(), lat, lng, alt, mr_pwm, ml_pwm);
    return _logBuffer;
}

bool Logger::sdInit() {
    if (!_sd.begin()) {
        return false;
    }
    return true;
}

bool Logger::createLogFile() {
    _myFile = _sd.open("log/log.csv", FILE_WRITE);
    if (!_myFile) {
        return false;
    }
    _myFile.println(CSV_HEADER);
    _myFile.close();
    return true;
}

// JPEGファイルの保存
bool Logger::saveJPEGImage(void* buff, size_t size) {
    _myFile = _sd.open(_jpegFileName, FILE_WRITE);
    if (!_myFile) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    _myFile.write((uint8_t*)buff, size);
    _myFile.close();

    shiftJPEGFileName();
    return true;
}

//_jpegFileNameCountの開始番号を決める
void Logger::refreshJPEGFileNameIndex() {
    while (true) {
        shiftJPEGFileName();
        if (!_sd.exists(_jpegFileNameCount)) {
            break;
        }
    }
}

// JPEGファイルのインクリメント
void Logger::shiftJPEGFileName() {
    sprintf(_jpegFileName, "/explore_%04d.jpg", _jpegFileNameCount);
    _jpegFileNameCount++;
}

// TODO: PPMファイルの保存
bool Logger::savePPMImage(void* buff, size_t size) {
    _myFile = _sd.open(_ppmFileName, FILE_WRITE);
    if (!_myFile) {
        Serial.println("Failed to open file for writing");
        return false;
    }

    _myFile.printf("P6\n%lu %lu\n255\n", 96, 96);

    _myFile.write((uint8_t*)buff, size);
    _myFile.close();
    
    shiftPPMFileName();
    return true;
}

// TODO: _ppmFileNameCountの開始番号を決める
void Logger::refreshPPMFileNameIndex() {
    while (true) {
        shiftPPMFileName();
        if (!_sd.exists(_ppmFileNameCount)) {
            break;
        }
    }
}

// TODO: PPMファイルのインクリメント
void Logger::shiftPPMFileName() {
    sprintf(_ppmFileName, "/detection_%4d.ppm", _ppmFileNameCount);
    _ppmFileNameCount++;
}