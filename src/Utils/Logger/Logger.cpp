#include "Logger.hpp"
#include <Arduino.h>

#define TweliteSend(c) Serial2.println(c)

Logger::Logger() : _sd(), _myFile() {}

bool Logger::begin() {
    if (!sdInit()) {
        return false;
    }
    
    if (!createLogFile()) {
        return false;
    }

    refreshFilenameIndex();
    
    return true;
}

bool Logger::appendLog(const char* message) {
    _myFile = _sd.open("log/log.csv", FILE_WRITE);
    
    if (!_myFile) {
        return false;
    }
    
    _myFile.println(message);
    _myFile.close();
    
    Serial.println(message);
    tweliteSend(message); // 無線でログを送信
    
    return true;
}

const char* Logger::createMessage(unsigned long currentTime, const String& currentDate, 
                           int state, double lat, double lng, double alt,
                           double distance, double direction, int mr_pwm, int ml_pwm,
                           int mOutputTime, int cds, double ax, double ay, double az,
                           double gx, double gy, double gz, double mx, double my, double mz,
                           double roll, double pitch, double heading) {
    // snprintfでフォーマットされた文字列を生成
    // 注意: AVRベースのArduinoでは、浮動小数点数のサポートに特別な設定が必要な場合があります
    snprintf(_logBuffer, sizeof(_logBuffer),
             "%lu,%s,%d,%.6f,%.6f,%.2f,%.2f,%.2f,%d,%d,%d,%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.2f,%.2f,%.2f",
             currentTime,
             currentDate.c_str(),
             state,
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

const char* Logger::createMessage(const String& currentDate, int state,
                                  double lat, double lng, double alt,
                                  int mr_pwm, int ml_pwm) {
    snprintf(_logBuffer, sizeof(_logBuffer),
             "%s,%d,%.6f,%.6f,%.2f,%d,%d",
             currentDate.c_str(), state, lat, lng, alt, mr_pwm, ml_pwm);
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

void Logger::tweliteSend(const char* message) {
    TweliteSend(message);
}

bool Logger::saveImage(void* buff, size_t size) {
    File myFile = _sd.open(_imageFilename, FILE_WRITE);
    if (!myFile) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    myFile.write((uint8_t*)buff, size);
    myFile.close();

    shiftImageFilename();
    return true;
}

void Logger::shiftImageFilename() {
    sprintf(_imageFilename, "/Image_%04d.jpg", _imageNameCount);
    _imageNameCount++;
}

void Logger::refreshFilenameIndex() {
    while (true) {
        shiftImageFilename();
        if (!_sd.exists(_imageFilename)) {
            break;
        }
    }
}