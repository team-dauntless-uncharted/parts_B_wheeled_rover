#pragma once
#include "Sensor/Gnss/GnssSensor.hpp"
#include "Sensor/Imu/ImuSensor.hpp"
#include "Sensor/CdS/CdSSensor.hpp"
#include "Sensor/Camera/CameraController.hpp"

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
#include <TwelitePacket.h>

#include "Controller/ICansatState.hpp"
#include <array>

#define SENSOR_BUFFER_SIZE 4096 

struct UserConfig {
    double goalLat;
    double goalLng;

    double standbyStateAltThreshold;            // STANDBYでの高度閾値
    unsigned long standbyStateTimeoutThreshold; // STANDBYでのタイムアウト待ち時間
    int launchStateCdsThreshold;                // LAUNCHでのCdS閾値
    unsigned long launchStateTimeoutThreshold;  // LAUNCHでのタイムアウト待ち時間
    unsigned long dropStateTimeoutThreshold;    // DROPでのタイムアウト待ち時間
    double escapeStateDistanceThreshold;        // ESCAPEでの距離閾値(メートル)
    unsigned long escapeStateTimeoutThreshold;  // ESCAPEでのタイムアウト待ち時間
    int detectionMaxFailedCount;                // DETECTIONでの失敗上限回数
    unsigned long recordingTimeoutThreshold;    // RECORDINGでのタイムアウト待ち時間
};

class CansatController {
public:
    CansatController();
    void begin(UserConfig config);
    void update();
    void changeState(std::unique_ptr<ICansatState> newState);

    void writeSystemLog(const char* message);

    // センサアクセス
    GnssSensor &getGnss() { return _gnss; }
    ImuSensor &getImu() { return _imu; }
    CdSSensor &getCds() { return _cds; }
    CameraController &getCamera() { return _camera; }
    PowerController &getPower() { return _power; }

    // アクチュエータアクセス
    Motor &getMotor() { return _motor; }
    Led &getLed(int idx) { return _led[idx]; }
    Heater &getHeater() { return _heater; }
    Speaker &getSpeaker() { return _speaker; }

    // データのやり取り
    twelite::TwelitePacket &getTwelite() { return _twelite; }
    SDLogger &getSDLogger() { return _sdLogger; }
#ifdef USE_FLASH
    FlashIO &getFlashIO() { return _flash; }
#endif // USE_FLASH
    SerialWriter &getSerialWriter() { return _writer; }
    
    void setInitTwelite(bool initTwelite) { _initTwelite = initTwelite; }
    void setInitCamera(bool initCamera) { _initCamera = initCamera; }

    bool isInitTwelite() { return _initTwelite; }
    bool isInitCamera() { return _initCamera; }

    UserConfig getUserConfig() { return _config; }
    
    void appendSensorLog();

    void setLed(const int state);

private:
    UserConfig _config;

    void configState();

    // CSVファイル
    const String CSV_HEADER = "time,date,mode,lat,lng,alt,cds,ax,ay,az,gx,gy,gz,mx,my,mz,roll,pitch,heading,voltage";
    char _sensorBuffer[SENSOR_BUFFER_SIZE];
    size_t _head = 0;

    std::unique_ptr<ICansatState> _state;
    
    bool _initTwelite = false;
    bool _initCamera = false;
    
    // センサ
    GnssSensor _gnss;
    ImuSensor _imu;
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
    twelite::TwelitePacket _twelite;
    SDLogger _sdLogger;
#ifdef USE_FLASH
    FlashIO _flash;
#endif // USE_FLASH
    SerialWriter _writer;
};