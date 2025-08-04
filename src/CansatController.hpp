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
#include "Utils/Logger/Logger.hpp"
#include "Utils/Twelite/TweliteController.hpp"

enum class CansatState {
    CALIBRATION,
    STAND_BY,
    LAUNCH,
    DROP,
    LANDING,
    NAVIGATION,
    GOAL,
};

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
    void runState();
    CansatState state;
    UserConfig userConfig;
    void waitMessage();
    
    // センサ値の取得メソッド
    double getCurrentAlt() const;
    double getDistanceToGoal() const;
    double getDirectionToGoal() const;
    double getHeading() const;
    int getCdsValue() const;
    double getAcceleration() const;
    
private:
    // 状態処理メソッド
    void handleCalibration();
    void handleStandBy();
    void handleLaunch();
    void handleDrop();
    void handleLanding();
    void handleNavigation();
    void handleGoal();
    
    // ログ出力
    void appendLog();
    
    // 状態管理用メンバ変数
    bool _altFlag;
    bool _timeFlag;
    bool _cdsFlag;
    bool _accFlag;
    long _currentTime;
    int _mOutputTime;
    int _mr_pwm;
    int _ml_pwm;
    
    // センサ・アクチュエータ
    GnssSensor _gnss;
    ImuSensor _imu;
    CdSSensor _cds;
    int _motorR_pins[3];
    int _motorL_pins[3];
    Motor _motor;
    Led _led0, _led1, _led2, _led3;
    Speaker _speaker;
    Heater _heater;
    Logger _logger;
    TweliteController _twelite;

    CameraController _camera;
    
    // 計算値
    double _distanceToGoal;
    double _directionToGoal;
};