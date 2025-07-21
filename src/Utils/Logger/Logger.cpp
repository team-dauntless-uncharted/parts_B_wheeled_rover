#include "Logger.hpp"
#include <Arduino.h>

#define TweliteSend(c) Serial2.print(c)

Logger::Logger() : _sd(), _myFile() {}

bool Logger::begin() {
    if (!sdInit()) {
        return false;
    }
    
    if (!createLogFile()) {
        return false;
    }
    
    return true;
}

bool Logger::appendLog(const String& message) {
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

String Logger::createMessage(unsigned long currentTime, const String& currentDate, 
                           int state, double lat, double lng, double alt,
                           double distance, double direction, int mr_pwm, int ml_pwm,
                           int mOutputTime, int cds, double ax, double ay, double az,
                           double gx, double gy, double gz, double mx, double my, double mz,
                           double roll, double pitch, double heading) {
    // 300バイトのバッファを確保。Stringの連結によるメモリの断片化を避ける
    char message[300];

    // snprintfでフォーマットされた文字列を生成
    // 注意: AVRベースのArduinoでは、浮動小数点数のサポートに特別な設定が必要な場合があります
    snprintf(message, sizeof(message),
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

    return String(message);
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

void Logger::tweliteSend(const String& message) {
    TweliteSend(message);
} 