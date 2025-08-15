#pragma once
#include "Sensor/Gnss/GnssSensor.hpp"
#include "Sensor/Imu/ImuSensor.hpp"
#include "Sensor/CdS/CdSSensor.hpp"
#include "Sensor/Camera/CameraController.hpp"
#include "Actuator/Motor/Motor.hpp"
#include "Actuator/Led/Led.hpp"
#include "Actuator/Speaker/Speaker.hpp"
#include "Actuator/Heater/Heater.hpp"
#include "Utils/GeoUtils/GeoUtils.hpp"
#include "Utils/Twelite/Twelite.hpp"
#include "Utils/Logger/Logger.hpp"
#include "Utils/Serial/SerialWriter.hpp"

#include "Controller/ICansatState.hpp"

#include <array>

struct UserConfig {
    double goalLat;
    double goalLng;
    int altThreshold;
    long timeThreshold;
    int cdsThreshold;
    double accThreshold;
    int distanceThreshold;
};

class CansatController {
public:
    CansatController();
    void begin();
    void update();
    void changeState(std::unique_ptr<ICansatState> newState);
    UserConfig userConfig;

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
    TweliteController &getTwelite() { return _twelite; }
    Logger &getLogger() { return _logger; }
    SerialWriter &getSerialWriter() { return _writer; }
    
    // センサ値の取得メソッド
    double getCurrentAlt() const;
    double getDistanceToGoal() const;
    double getDirectionToGoal() const;
    double getHeading() const;
    int getCdsValue() const;
    double getAcceleration() const;

    void setAltFlag(bool altFlag) { _altFlag = altFlag; }
    void setTimeFlag(bool timeFlag) { _timeFlag = timeFlag; }
    void setCdsFlag(bool cdsFlag) { _cdsFlag = cdsFlag; }
    void setAccFlag(bool accFlag) { _accFlag = accFlag; }
    void setCurrentTime(long currentTime) { _currentTime = currentTime; }
    void setmOutputTime(int mOutputTime) { _mOutputTime = mOutputTime; }
    void setMrPwm(int mrPwm) { _mr_pwm = mrPwm; }
    void setMlPwm(int mlPwm) { _ml_pwm = mlPwm; }

    bool getAltFlag() { return _altFlag; }
    bool getTimeFlag() { return _timeFlag; }
    bool getCdsFlag() { return _cdsFlag; }
    bool getAccFlag() { return _accFlag; }
    long getCurrentTime() { return _currentTime; }
    int getmOutputTime() { return _mOutputTime; }
    int getMrPwm() { return _mr_pwm; }
    int getMlPwm() { return _ml_pwm; }
    
private:
    // ログ出力
    void appendSensorLog();

    const char* createMessage(unsigned long currentTime, const String& currentDate, State state,
                        double lat, double lng, double alt,
                        double distance, double direction, int mr_pwm, int ml_pwm,
                        int mOutputTime, int cds, double ax, double ay, double az,
                        double gx, double gy, double gz, double mx, double my, double mz,
                        double roll, double pitch, double heading);

    // CSVファイル
    const String CSV_HEADER = "time,date,mode,lat,lng,alt,distance,direction,mr_pwm,ml_pwm,mOutputTime,cds,ax,ay,az,gx,gy,gz,mx,my,mz,roll,pitch,heading";
    char _logBuffer[300];

    std::unique_ptr<ICansatState> _state;
    
    // 状態管理用メンバ変数
    bool _altFlag;
    bool _timeFlag;
    bool _cdsFlag;
    bool _accFlag;
    long _currentTime;
    int _mOutputTime;
    int _mr_pwm;
    int _ml_pwm;
    
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

    // データのやり取り
    TweliteController _twelite;
    Logger _logger;
    SerialWriter _writer;

    // 計算値
    double _distanceToGoal;
    double _directionToGoal;
};