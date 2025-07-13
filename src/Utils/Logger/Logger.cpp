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
    
    tweliteSend(message); // 無線でログを送信
    
    return true;
}

String Logger::createMessage(unsigned long currentTime, const String& currentDate, 
                           int state, double lat, double lng, double alt,
                           double distance, double direction, int mr_pwm, int ml_pwm,
                           int mOutputTime, int cds, double ax, double ay, double az,
                           double gx, double gy, double gz, double mx, double my, double mz,
                           double roll, double pitch, double heading) {
    String message = "";
    message += String(currentTime);     message += ","; // time
    message += currentDate;             message += ","; // date
    message += String(state);           message += ","; // mode
    message += String(lat);             message += ","; // lat
    message += String(lng);             message += ","; // lng
    message += String(alt);             message += ","; // alt
    message += String(distance);        message += ","; // distance
    message += String(direction);       message += ","; // direction
    message += String(mr_pwm);          message += ","; // mr_pwm
    message += String(ml_pwm);          message += ","; // ml_pwm
    message += String(mOutputTime);     message += ","; // mOutputTime
    message += String(cds);             message += ","; // cds
    message += String(ax);              message += ","; // ax
    message += String(ay);              message += ","; // ay
    message += String(az);              message += ","; // az
    message += String(gx);              message += ","; // gx
    message += String(gy);              message += ","; // gy
    message += String(gz);              message += ","; // gz
    message += String(mx);              message += ","; // mx
    message += String(my);              message += ","; // my
    message += String(mz);              message += ","; // mz
    message += String(roll);            message += ","; // roll
    message += String(pitch);           message += ","; // pitch
    message += String(heading);         message += ","; // heading
    
    return message;
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