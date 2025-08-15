#include <Arduino.h>
#include "CansatController.hpp"
#include "Controller/States/CalibrationState.hpp"

CansatController::CansatController()
    : userConfig{35.7100152, 139.8107594, 20, 0, 400, 0, 5},
      _altFlag(false), _timeFlag(false), _cdsFlag(false), _accFlag(false),
      _currentTime(0), _mOutputTime(0), _mr_pwm(0), _ml_pwm(0),
      _motorR_pins{8, 4, 5},
      _motorL_pins{7, 2, 3},
      _cds(A0),
      _motor(_motorR_pins, _motorL_pins),
      _led{Led(0), Led(1), Led(2), Led(3)},
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
    _writer.begin();
    _writer.log("CansatController: Starting begin()");
    
    _writer.log("CansatController: Initializing GNSS...");
    if (!_gnss.begin()) {
        _writer.log("CansatController: GNSS initialization failed!");
    } else {
        _writer.log("CansatController: GNSS initialized successfully");
    }

    _writer.log("CansatController: Waiting for GNSS position fix...");
    if (!_gnss.waitReceive()) {
        _writer.log("CansatController: GNSS position fix failed!");
    } else {
        _writer.log("CansatController: GNSS position fix succeeded");
    }
    
    _writer.log("CansatController: Initializing IMU...");
    if (!_imu.begin()) {
        _writer.log("CansatController: IMU initialization failed!");
    } else {
        _writer.log("CansatController: IMU initialized successfully");
    }
    
    _writer.log("CansatController: Initializing Logger...");
    if (!_logger.begin(CSV_HEADER)) {
        _writer.log("CansatController: Logger initialization failed!");
    } else {
        _writer.log("CansatController: Logger initialized successfully");
    }

    // init camera
    _writer.log("Prepare camera");
    if (!_camera.begin(DETECTION_MODE)) {
        _writer.log("Camera init failed");
    }
    
    _writer.log("Camera setup completed for Cansat landing site capture");
    _writer.log("Start streaming");
    if (!_camera.startStreaming(true)) {
        _writer.log("Failed to start streaming");
    }
    
    _currentTime = millis();
    
    // 初期メッセージ送信
    _writer.log("CansatController: Initial message sent");
    
    _writer.log("CansatController: begin() completed");

    _speaker.playStart();

    changeState(std::make_unique<CalibrationState>(*this));
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

    appendLog();

    if (_state) _state->onUpdate();
}

void CansatController::changeState(std::unique_ptr<ICansatState> newState) {
    if (_state) _state->onExit();
    _state = std::move(newState);
    if (_state) _state->onEnter();
}

// ログの作成
const char* CansatController::createMessage(unsigned long currentTime, const String& currentDate, State state,
                           double lat, double lng, double alt,
                           double distance, double direction, int mr_pwm, int ml_pwm,
                           int mOutputTime, int cds, double ax, double ay, double az,
                           double gx, double gy, double gz, double mx, double my, double mz,
                           double roll, double pitch, double heading) {
    // snprintfでフォーマットされた文字列を生成
    // 注意: AVRベースのArduinoでは、浮動小数点数のサポートに特別な設定が必要な場合があります
    snprintf(_logBuffer, sizeof(_logBuffer),
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

    return _logBuffer;
}

void CansatController::appendLog() {
    char *message = createMessage(
        millis(), _gnss.getCurrentDate(), _state->getState(),
        _gnss.getLatitude(), _gnss.getLongitude(), _gnss.getAltitude(),
        _distanceToGoal, _directionToGoal, _mr_pwm, _ml_pwm, _mOutputTime,
        getCdsValue(), _imu.getAccX(), _imu.getAccY(), _imu.getAccZ(),
        _imu.getGyroX(), _imu.getGyroY(), _imu.getGyroZ(),
        _imu.getMagX(), _imu.getMagY(), _imu.getMagZ(),
        _imu.getRoll(), _imu.getPitch(), _imu.getHeading()
    );

    _writer.log(message);
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