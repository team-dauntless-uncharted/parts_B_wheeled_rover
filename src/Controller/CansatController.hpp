#pragma once
#include "Sensor/Gnss/GnssSensor.hpp"
#include "Sensor/CdS/CdSSensor.hpp"
#include "Sensor/Camera/CameraController.hpp"
#include <BNO055Library.h>

#include "Actuator/Motor/Motor.hpp"
#include "Actuator/Led/Led.hpp"
#include "Actuator/Speaker/Speaker.hpp"
#include "Actuator/Heater/Heater.hpp"

#include "Utils/FileIO/SDLogger.hpp"
#ifdef USE_FLASH
#include "Utils/FileIO/FlashIO.hpp"
#endif // USE_FLASH
#include "Utils/PowerController/PowerController.hpp"
#include "Utils/Serial/SerialWriter.hpp"

#include "Controller/ICansatState.hpp"
#include <array>
#include <cstdarg>
#include <cstdio>

#define SENSOR_BUFFER_SIZE 4096 

struct UserConfig {
    unsigned long calibrationStateTimeoutThreshold = 5 * 60 * 1000; // CALIBRATIONでのタイムアウト待ち時間
    unsigned long standbyStateTimeoutThreshold = 10 * 60 * 1000;    // STANDBYでのタイムアウト待ち時間
    int launchStateCdsThreshold = 400;                              // LAUNCHでのCdS閾値
    unsigned long launchStateTimeoutThreshold = 20 * 60 * 1000;     // LAUNCHでのタイムアウト待ち時間
    unsigned long dropStateTimeoutThreshold = 10 * 60 * 1000;       // DROPでのタイムアウト待ち時間
    unsigned long escapeStateTimeoutThreshold = 15 * 1000;          // ESCAPEでのタイムアウト待ち時間
    int detectionMaxFailedCount = 25;                               // DETECTIONでの失敗上限回数
    int detectionTurn180delay = 700;                                // DETECIIONで180度回転するのにかかる秒数
    unsigned long recordingTimeoutThreshold = 5 * 60 * 1000;        // RECORDINGでのタイムアウト待ち時間
    int recordingTime = 30 * 1000;                                  // RECORDINGでの時間(ミリ秒)
};

class CansatController {
public:
    CansatController();
    void begin();
    void update();
    void changeState(std::unique_ptr<ICansatState> newState);

    void writeSystemLog(const char* format, ...);

    // センサアクセス
    GnssSensor &getGnss() { return _gnss; }
    BNO055 &getBno055() { return _bno055; }
    CdSSensor &getCds() { return _cds; }
    CameraController &getCamera() { return _camera; }
    PowerController &getPower() { return _power; }

    // アクチュエータアクセス
    Motor &getMotor() { return _motor; }
    Led &getLed(int idx) { return _led[idx]; }
    Heater &getHeater() { return _heater; }
    Speaker &getSpeaker() { return _speaker; }

    // データのやり取り
    SDLogger &getSDLogger() { return _sdLogger; }
#ifdef USE_FLASH
    FlashIO &getFlashIO() { return _flash; }
#endif // USE_FLASH
    SerialWriter &getSerialWriter() { return _writer; }
    
    void setInitCamera(bool initCamera) { _initCamera = initCamera; }

    bool isInitCamera() { return _initCamera; }

    UserConfig getUserConfig() { return _config; }
    
    void appendSensorLog();

    void setLed(const int state);

private:
    UserConfig _config;
    unsigned long _lastHeartBeat = 0;

    void dumpConfig();
    void readConfigFile();
    void configState();

    // CSVファイル
    const String CSV_HEADER = "time,date,mode,lat,lng,alt,cds,ax,ay,az,gx,gy,gz,mx,my,mz,roll,pitch,heading,voltage";
    char _sensorBuffer[SENSOR_BUFFER_SIZE];
    size_t _head = 0;

    std::unique_ptr<ICansatState> _state;
    
    bool _initCamera = false;
    
    // センサ
    GnssSensor _gnss;
    BNO055 _bno055;
    Vector<float> _acceleration;
    Vector<float> _gyro;
    Vector<float> _magnetic;
    EulerAngles _euler;
    CdSSensor _cds;
    CameraController _camera;

    // アクチュエータ
    int _motorR_pins[3];
    int _motorL_pins[3];
    Motor _motor;
    std::array<Led, 4> _led;
    Speaker _speaker;
    Heater _heater;

    PowerController _power;

    // データのやり取り
    SDLogger _sdLogger;
#ifdef USE_FLASH
    FlashIO _flash;
#endif // USE_FLASH
    SerialWriter _writer;
};