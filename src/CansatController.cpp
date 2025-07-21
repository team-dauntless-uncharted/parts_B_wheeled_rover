#include "CansatController.hpp"
#include <Arduino.h>

CansatController::CansatController()
    : state(CansatState::NAVIGATION),
      userConfig{35.7100152, 139.8107594, 20, 0, 400, 0, 5},
      _altFlag(false), _timeFlag(false), _cdsFlag(false), _accFlag(false),
      _currentTime(0), _mOutputTime(0), _mr_pwm(0), _ml_pwm(0),
      _motorR_pins{8, 4, 5},
      _motorL_pins{7, 2, 3},
      _cds(A0),
      _motor(_motorR_pins, _motorL_pins),
      _led0(0), _led1(1), _led2(2), _led3(3),
      _speaker(9),
      _heater(6),
      _logger(),
      _distanceToGoal(10000.0),
      _directionToGoal(0.0)
{
}

void CansatController::begin() {
    Serial.println("CansatController: Starting begin()");
    
    Serial.println("CansatController: Initializing GNSS...");
    if (!_gnss.begin()) {
        Serial.println("CansatController: GNSS initialization failed!");
    } else {
        Serial.println("CansatController: GNSS initialized successfully");
    }
    
    Serial.println("CansatController: Initializing IMU...");
    if (!_imu.begin()) {
        Serial.println("CansatController: IMU initialization failed!");
    } else {
        Serial.println("CansatController: IMU initialized successfully");
    }
    
    Serial.println("CansatController: Initializing Logger...");
    if (!_logger.begin()) {
        Serial.println("CansatController: Logger initialization failed!");
    } else {
        Serial.println("CansatController: Logger initialized successfully");
    }
    
    _currentTime = millis();
    
    // 無線通信初期化
    Serial.println("CansatController: Initializing wireless communication...");
    Serial2.begin(115200);
    while(!Serial2) {}
    Serial.println("CansatController: Wireless communication initialized");
    
    // 初期メッセージ送信
    Serial2.print("Hello 100kinSAT!!!\n");
    Serial.println("CansatController: Initial message sent");
    
    Serial.println("CansatController: begin() completed");
}

void CansatController::update() {
    _gnss.update();
    _imu.update();
    _cds.read();
    
    // 距離・方位の計算
    _distanceToGoal = GeoUtils::haversineDistance(
        _gnss.getLatitude(), _gnss.getLongitude(), 
        userConfig.goalLat, userConfig.goalLng
    );
    _directionToGoal = GeoUtils::haversineBearing(
        _gnss.getLatitude(), _gnss.getLongitude(), 
        userConfig.goalLat, userConfig.goalLng
    );
}

void CansatController::runState() {
    switch (state) {
        case CansatState::CALIBRATION:
            handleCalibration();
            break;
        case CansatState::STAND_BY:
            handleStandBy();
            break;
        case CansatState::LAUNCH:
            handleLaunch();
            break;
        case CansatState::DROP:
            handleDrop();
            break;
        case CansatState::LANDING:
            handleLanding();
            break;
        case CansatState::NAVIGATION:
            handleNavigation();
            break;
        case CansatState::GOAL:
            handleGoal();
            break;
        default:
            break;
    }
}

void CansatController::handleCalibration() {
    // IMUセンサのキャリブレーション
    // 高度のキャリブレーション（地表をゼロメートルに合わせる）
    update();
    appendLog();
}

void CansatController::handleStandBy() {
    // 一定の高度を超えたら LAUNCH モードに遷移する
    update();
    appendLog();

    if (getCurrentAlt() > userConfig.altThreshold) {
        _altFlag = true;
    }

    long elapsedTime = millis() - _currentTime;
    if (elapsedTime > userConfig.timeThreshold) {
        _timeFlag = true;
    }

    // 高度または時間の条件を満たしたらモード変更
    if (_altFlag || _timeFlag) {
        state = CansatState::LAUNCH;
    }
}

void CansatController::handleLaunch() {
    // 放出を検知したら DROP モードに遷移する
    update();
    appendLog();

    if (getCdsValue() < userConfig.cdsThreshold) {
        _cdsFlag = true;
    }

    if (_cdsFlag) {
        state = CansatState::DROP;
    }
}

void CansatController::handleDrop() {
    // 着地を検知したら LANDING モードに遷移する
    update();
    appendLog();

    // 加速度センサのxyz軸の平方和を計算
    double acc = getAcceleration();

    // しきい値以下になったら着地と判断する
    if (acc < userConfig.accThreshold) {
        _accFlag = true;
    }

    if (_accFlag) {
        state = CansatState::LANDING;
    }
}

void CansatController::handleLanding() {
    // パラシュートの切り離し
    update();
    appendLog();

    delay(5000);

    // ニクロム線を加熱してテグスを切る
    _heater.heat(150, 10000);

    delay(5000);

    state = CansatState::NAVIGATION;
}

void CansatController::handleNavigation() {
    // 目標地点に到達したら GOAL モードに遷移する
    update();
    appendLog();

    // 移動前のゴールとの距離を取得
    double beforeDistance = getDistanceToGoal();
    // ゴールとの距離がしきい値以下の場合、ゴール状態へ遷移
    if (beforeDistance < userConfig.distanceThreshold) {
        state = CansatState::GOAL;
        return;
    }

    // 移動前のゴールとの方位を取得
    double beforeDirection = getDirectionToGoal();
    // CanSatの向きと比較
    double courseDiff = beforeDirection - getHeading();
    // ゴールへ方向転換
    _mOutputTime = (int)(14 * abs(courseDiff)); // モータへの出力時間を求める
    if (courseDiff > 0) {
        _motor.turnRight(150);
        _mr_pwm = 150; _ml_pwm = 150;
        delay(_mOutputTime);
        _motor.stop();
        _mr_pwm = 0; _ml_pwm = 0;
    } else {
        _motor.turnLeft(150);
        _mr_pwm = 150; _ml_pwm = 150;
        delay(_mOutputTime);
        _motor.stop();
        _mr_pwm = 0; _ml_pwm = 0;
    }

    update();
    appendLog();

    // 直進する
    _mOutputTime = 5000;
    _motor.forward(200);
    _mr_pwm = 200; _ml_pwm = 200;
    delay(_mOutputTime);
    _motor.stop();
    _mr_pwm = 0; _ml_pwm = 0;

    update();
    appendLog();

    _mOutputTime = 0;
}

void CansatController::handleGoal() {
    // だんご大家族を演奏する
    // LED2を点灯する
    update();
    appendLog();

    delay(2000);
    // playDango(); // TODO: 実装が必要
    while(1);
}

void CansatController::appendLog() {
    char *message = _logger.createMessage(
        millis(), _gnss.getCurrentDate(), (int)state,
        _gnss.getLatitude(), _gnss.getLongitude(), _gnss.getAltitude(),
        _distanceToGoal, _directionToGoal, _mr_pwm, _ml_pwm, _mOutputTime,
        getCdsValue(), _imu.getAccX(), _imu.getAccY(), _imu.getAccZ(),
        _imu.getGyroX(), _imu.getGyroY(), _imu.getGyroZ(),
        _imu.getMagX(), _imu.getMagY(), _imu.getMagZ(),
        _imu.getRoll(), _imu.getPitch(), _imu.getHeading()
    );
    _logger.appendLog(message);
}

// センサ値取得メソッド
double CansatController::getCurrentAlt() const {
    return _gnss.getAltitude();
}

double CansatController::getDistanceToGoal() const {
    return _distanceToGoal;
}

double CansatController::getDirectionToGoal() const {
    return _directionToGoal;
}

double CansatController::getHeading() const {
    return _imu.getHeading();
}

int CansatController::getCdsValue() const {
    return _cds.read();
}

double CansatController::getAcceleration() const {
    double accX = _imu.getAccX();
    double accY = _imu.getAccY();
    double accZ = _imu.getAccZ();
    return accX * accX + accY * accY + accZ * accZ;
}