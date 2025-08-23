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
#include "Utils/Serial/SerialWriter.hpp"
#include "Utils/PowerController/PowerController.hpp"
#include <TwelitePacket.h>

#include "Controller/ICansatState.hpp"

#include <array>

#define SENSOR_BUFFER_SIZE 4096 

struct UserConfig {
    double goalLat;
    double goalLng;

    double standbyStateAltThreshold;
    long standbyStateTimeThreshold;
    int launchStateCdsThreshold;
    long dropStateTimeThreshold;
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

    // アクチュエータアクセス
    Motor &getMotor() { return _motor; }
    Led &getLed(int idx) { return _led[idx]; }
    Heater &getHeater() { return _heater; }
    Speaker &getSpeaker() { return _speaker; }

    // データのやり取り
    twelite::TwelitePacket &getTwelite() { return _twelite; }
    SDLogger &getSDLogger() { return _sdLogger; }
    SerialWriter &getSerialWriter() { return _writer; }
    
    void setAltFlag(bool altFlag) { _altFlag = altFlag; }
    void setTimeFlag(bool timeFlag) { _timeFlag = timeFlag; }
    void setCdsFlag(bool cdsFlag) { _cdsFlag = cdsFlag; }
    void setAccFlag(bool accFlag) { _accFlag = accFlag; }
    void setInitTwelite(bool initTwelite) { _initTwelite = initTwelite; }

    bool getAltFlag() { return _altFlag; }
    bool getTimeFlag() { return _timeFlag; }
    bool getCdsFlag() { return _cdsFlag; }
    bool getAccFlag() { return _accFlag; }
    bool isInitTwelite() { return _initTwelite; }

    UserConfig getUserConfig() { return _config; }
    
    void appendSensorLog();

private:
    UserConfig _config;

    void configState();

    // CSVファイル
    const String CSV_HEADER = "time,date,mode,lat,lng,alt,distance,direction,mr_pwm,ml_pwm,mOutputTime,cds,ax,ay,az,gx,gy,gz,mx,my,mz,roll,pitch,heading,voltage";
    char _sensorBuffer[SENSOR_BUFFER_SIZE];
    size_t _head = 0;

    std::unique_ptr<ICansatState> _state;
    
    // 状態管理用メンバ変数
    bool _altFlag = false;
    bool _timeFlag = false;
    bool _cdsFlag = false;
    bool _accFlag = false;

    bool _initTwelite = false;
    
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
    SerialWriter _writer;
};