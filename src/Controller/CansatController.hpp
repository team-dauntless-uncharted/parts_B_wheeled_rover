#pragma once
#include "Sensor/Gnss/GnssSensor.hpp"
#include "Sensor/Imu/ImuSensor.hpp"
#include "Sensor/CdS/CdSSensor.hpp"
#include "Sensor/Camera/CameraController.hpp"
#include "Actuator/Motor/Motor.hpp"
#include "Actuator/Led/Led.hpp"
#include "Actuator/Speaker/Speaker.hpp"
#include "Actuator/Heater/Heater.hpp"
#include "Utils/Logger/Logger.hpp"
#include "Utils/Serial/SerialWriter.hpp"
#include "Utils/PowerController/PowerController.hpp"
#include <TwelitePacket.h>

#include "Controller/ICansatState.hpp"

#include <array>

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
    Logger &getLogger() { return _logger; }
    SerialWriter &getSerialWriter() { return _writer; }
    
    void setAltFlag(bool altFlag) { _altFlag = altFlag; }
    void setTimeFlag(bool timeFlag) { _timeFlag = timeFlag; }
    void setCdsFlag(bool cdsFlag) { _cdsFlag = cdsFlag; }
    void setAccFlag(bool accFlag) { _accFlag = accFlag; }

    bool getAltFlag() { return _altFlag; }
    bool getTimeFlag() { return _timeFlag; }
    bool getCdsFlag() { return _cdsFlag; }
    bool getAccFlag() { return _accFlag; }

    UserConfig getUserConfig() { return _config; }
    
    void appendSensorLog();

private:
    UserConfig _config;

    void configState();

    // ログ出力
    const char* createMessage(unsigned long currentTime, const String& currentDate, State state,
                        double lat, double lng, double alt,
                        int cds, double ax, double ay, double az,
                        double gx, double gy, double gz, double mx, double my, double mz,
                        double roll, double pitch, double heading, int voltage);

    // CSVファイル
    const String CSV_HEADER = "time,date,mode,lat,lng,alt,distance,direction,mr_pwm,ml_pwm,mOutputTime,cds,ax,ay,az,gx,gy,gz,mx,my,mz,roll,pitch,heading,voltage";
    char _logBuffer[300];

    std::unique_ptr<ICansatState> _state;
    
    // 状態管理用メンバ変数
    bool _altFlag;
    bool _timeFlag;
    bool _cdsFlag;
    bool _accFlag;
    
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
    Logger _logger;
    SerialWriter _writer;
};