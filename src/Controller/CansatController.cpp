#include <Arduino.h>
#include "Controller/CansatController.hpp"
#include "Controller/States/CalibrationState.hpp"

CansatController::CansatController()
    : _altFlag(false), _timeFlag(false), _cdsFlag(false), _accFlag(false),
      _motorR_pins{8, 4, 5},
      _motorL_pins{7, 2, 3},
      _gnss(1000),
      _imu(),
      _cds(A0),
      _motor(_motorR_pins, _motorL_pins),
      _led{Led(0), Led(1), Led(2), Led(3)},
      _speaker(9),
      _heater(6),
      _twelite(),
      _logger(),
      _camera(),
      _power()
{
}

void CansatController::writeSystemLog(const char* message) {
    _logger.appendSystemLog(message);
    _writer.log(message);
}

void CansatController::begin(UserConfig config) {
    _config = config;

    _writer.begin();
    _writer.log("CansatController: Starting begin()");
    
    if (!_logger.begin(CSV_HEADER)) {
        _writer.log("CansatController: Logger initialization failed!");
    } else {
        _writer.log("CansatController: Logger initialized successfully");
    }
    _logger.appendSystemLog("CansatController: begin() started");

    _power.begin();

    if (!_gnss.begin()) {
        writeSystemLog("CansatController: GNSS initialization failed!");
    } else {
        writeSystemLog("CansatController: GNSS initialized successfully");
    }

    // if (!_gnss.waitReceive()) {
    //     writeSystemLog("CansatController: GNSS position fix failed!");
    // } else {
    //     writeSystemLog("CansatController: GNSS position fix succeeded");
    // }
    
    if (!_imu.begin()) {
        writeSystemLog("CansatController: IMU initialization failed!");
    } else {
        writeSystemLog("CansatController: IMU initialized successfully");
    }
    
    // init camera
    if (!_camera.begin(DETECTION_MODE)) {
        writeSystemLog("CansatController: Camera initialization failed!");
    } else {
        writeSystemLog("CansatController: Camera initialized successfully");
    }
    
    if (!_camera.startStreaming(true)) {
        writeSystemLog("CansatController: Failed to start streaming");
    } else {
        writeSystemLog("CansatController: Streaming started");
    }
    
    _speaker.playStart();

    changeState(std::make_unique<CalibrationState>(*this));
}

void CansatController::update() {
    _gnss.update();
    _imu.update();

    appendSensorLog();

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
                           int cds, double ax, double ay, double az,
                           double gx, double gy, double gz, double mx, double my, double mz,
                           double roll, double pitch, double heading, int voltage) {
    // snprintfでフォーマットされた文字列を生成
    // 注意: AVRベースのArduinoでは、浮動小数点数のサポートに特別な設定が必要な場合があります
    snprintf(_logBuffer, sizeof(_logBuffer),
             "%lu,%s,%d,%.6f,%.6f,%.2f,%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.2f,%.2f,%.2f,%d",
             currentTime,
             currentDate.c_str(),
             state,
             lat, lng, alt, cds,
             ax, ay, az,
             gx, gy, gz,
             mx, my, mz,
             roll, pitch, heading,
             voltage);

    return _logBuffer;
}

void CansatController::appendSensorLog() {
    char *message = createMessage(
        millis(), _gnss.getCurrentDate(), _state->getState(),
        _gnss.getLatitude(), _gnss.getLongitude(), _gnss.getAltitude(),
        _cds.read(), _imu.getAccX(), _imu.getAccY(), _imu.getAccZ(),
        _imu.getGyroX(), _imu.getGyroY(), _imu.getGyroZ(),
        _imu.getMagX(), _imu.getMagY(), _imu.getMagZ(),
        _imu.getRoll(), _imu.getPitch(), _imu.getHeading(), _power.getVoltage()
    );

    _writer.log(message);
    _logger.appendSensorLog(message);
}