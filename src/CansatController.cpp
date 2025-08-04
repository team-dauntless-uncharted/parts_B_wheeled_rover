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
      _twelite(),
      _logger(),
      _camera(),
      _distanceToGoal(10000.0),
      _directionToGoal(0.0)
{
}

void CansatController::begin() {
    Serial.println("CansatController: Starting begin()");
    
    // Serial.println("CansatController: Initializing GNSS...");
    // if (!_gnss.begin()) {
    //     Serial.println("CansatController: GNSS initialization failed!");
    // } else {
    //     Serial.println("CansatController: GNSS initialized successfully");
    // }
    
    // Serial.println("CansatController: Initializing IMU...");
    // if (!_imu.begin()) {
    //     Serial.println("CansatController: IMU initialization failed!");
    // } else {
    //     Serial.println("CansatController: IMU initialized successfully");
    // }
    
    Serial.println("CansatController: Initializing Logger...");
    if (!_logger.begin()) {
        Serial.println("CansatController: Logger initialization failed!");
    } else {
        Serial.println("CansatController: Logger initialized successfully");
    }

    // init camera
    // Serial.println("Prepare camera");
    // if (!_camera.begin()) {
    //     Serial.println("Camera init failed");
    //     return;
    // }
    
    // // 1. 高解像度設定（物体認識に適した解像度）
    // // 1280x960に設定
    // Serial.println("Setting high resolution for object detection...");
    // if (!_camera.setStillPictureImageFormat(CAM_IMGSIZE_VGA_H, CAM_IMGSIZE_VGA_V, CAM_IMAGE_PIX_FMT_JPG)) {
    //     Serial.println("Failed to set high resolution");
    // }
    
    // // 2. 高品質JPEG設定（機械学習の精度向上）
    // // 圧縮率が低いほど、画質が良くなる
    // Serial.println("Setting high JPEG quality...");
    // if (!_camera.setJPEGQuality(95)) {
    //     Serial.println("Failed to set JPEG quality");
    // }
    
    // // 3. ブレ軽減のための設定
    // // 自動ISO感度無効（手動制御）
    // // シャッタースピードを手動で設定するため、自動ISO感度を無効にする
    // if (!_camera.setAutoISOSensitivity(false)) {
    //     Serial.println("Failed to disable auto ISO");
    // }
    
    // // 高ISO感度で露光時間短縮（ブレ軽減）
    // // ISO感度が高いほど、暗い場所での撮影や、シャッタースピードを早くして手ブレを軽減することができる
    // if (!_camera.setISOSensitivity(CAM_ISO_SENSITIVITY_50)) {
    //     Serial.println("Failed to set ISO sensitivity");
    // }
    
    // // 自動露光無効（手動制御）
    // // シャッタースピードを手動で設定するため、自動露光を無効にする
    // if (!_camera.setAutoExposure(false)) {
    //     Serial.println("Failed to disable auto exposure");
    // }
    
    // // 短い露光時間設定（ブレ軽減）
    // // 露光時間が短いほど、ブレが軽減される
    // if (!_camera.setAbsoluteExposure(1000)) { // 100ms
    //     Serial.println("Failed to set exposure time");
    // }
    
    // // 4. ホワイトバランス設定（自然光下での正確な色再現）
    // Serial.println("Setting white balance for natural light...");
    // if (!_camera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT)) {
    //     Serial.println("Failed to set white balance");
    // }
    
    // // 5. 色効果無効（自然な色で物体認識）
    // Serial.println("Disabling color effects for natural colors...");
    // if (!_camera.setColorEffect(CAM_COLOR_FX_NONE)) {
    //     Serial.println("Failed to disable color effects");
    // }
    
    // Serial.println("Camera setup completed for Cansat landing site capture");
    // Serial.println("Start streaming");
    // if (!_camera.startStreaming()) {
    //     Serial.println("Failed to start streaming");
    // }
    
    _currentTime = millis();
    
    // 無線通信初期化
    Serial.println("CansatController: Initializing Twelite...");
    _twelite.begin();
    Serial.println("CansatController: Twelite initialized");
    // 初期メッセージは _twelite.begin() 内で送信されます
    Serial.println("CansatController: Initial message sent via Twelite");
    
    Serial.println("CansatController: begin() completed");

    _speaker.playStart();
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

    /**
     * TODO PHOTO
     */
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

    /**
     * TODO PHOTO
     */

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

    /**
     * 画像を撮影する
     */
    void* imgBuff = nullptr;
    size_t imgSize = 0;
    if (_camera.takePicture(&imgBuff, &imgSize)) {
        Serial.println("Save taken picture to SD card...");
        _logger.saveImage(imgBuff, imgSize);
    } else {
        Serial.println("Failed to take picture");
    }
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
    // char *message = _logger.createMessage(
    //     millis(), _gnss.getCurrentDate(), (int)state,
    //     _gnss.getLatitude(), _gnss.getLongitude(), _gnss.getAltitude(),
    //     _distanceToGoal, _directionToGoal, _mr_pwm, _ml_pwm, _mOutputTime,
    //     getCdsValue(), _imu.getAccX(), _imu.getAccY(), _imu.getAccZ(),
    //     _imu.getGyroX(), _imu.getGyroY(), _imu.getGyroZ(),
    //     _imu.getMagX(), _imu.getMagY(), _imu.getMagZ(),
    //     _imu.getRoll(), _imu.getPitch(), _imu.getHeading()
    // );

    char *message = _logger.createMessage(
        _gnss.getCurrentDate(), (int)state,
        _gnss.getLatitude(), _gnss.getLongitude(), _gnss.getAltitude(),
        _mr_pwm, _ml_pwm
    );
    _logger.appendLog(message);
    _twelite.sendMessage(message);
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

void CansatController::waitMessage() {
    Serial.println("Waiting for command...");
    int command = _twelite.receiveCommand(); // ブロックしてコマンドを待つ

    switch (command) {
        case 1: // 前進 (Forward)
            Serial.println("Command: Forward");
            _motor.forward(200);
            delay(1000); // 1秒間前進
            _motor.stop();
            break;
        case 2: // 後退 (Backward)
            Serial.println("Command: Backward");
            // _motor.backward(200);
            delay(1000); // 1秒間後退
            _motor.stop();
            break;
        case 3: // 左回転 (Left)
            Serial.println("Command: Turn Left");
            _motor.turnLeft(150);
            delay(500); // 0.5秒間左回転
            _motor.stop();
            break;
        case 4: // 右回転 (Right)
            Serial.println("Command: Turn Right");
            _motor.turnRight(150);
            delay(500); // 0.5秒間右回転
            _motor.stop();
            break;
        case 5: // カメラ撮影 (Camera)
            Serial.println("Command: Take Picture");
            // void* imgBuff = nullptr;
            // size_t imgSize = 0;
            // if (_camera.takePicture(&imgBuff, &imgSize)) {
            //     Serial.println("Save taken picture to SD card...");
            //     _logger.saveImage(imgBuff, imgSize);
            // } else {
            //     Serial.println("Failed to take picture");
            // }
            break;
        default:
            Serial.print("Unknown command received: ");
            Serial.println(command);
            break;
    }
}