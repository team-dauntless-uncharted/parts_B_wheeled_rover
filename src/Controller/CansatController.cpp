#include <Arduino.h>
#include "Controller/CansatController.hpp"

#include "Controller/States/CalibrationState.hpp"
#include "Controller/States/StandbyState.hpp"
#include "Controller/States/LaunchState.hpp"
#include "Controller/States/DropState.hpp"
#include "Controller/States/EscapeState.hpp"
#include "Controller/States/DetectionState.hpp"
#include "Controller/States/RecordingState.hpp"
#include "Controller/States/ExploreState.hpp"
#include "Controller/States/HelpingState.hpp"

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
    
    _speaker.playStart();

    configState();
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

void CansatController::configState() {
    State state;
    if (!_logger.readState((int&)state)) {
        // Flash
    }

    switch (state) {
    case State::CALIBRATION:
        writeSystemLog("config CalibrationState");
        changeState(std::make_unique<CalibrationState>(*this));
        break;
    case State::STANDBY:
        writeSystemLog("config StandbyState");
        changeState(std::make_unique<StandbyState>(*this));
        break;
    case State::LAUNCH:
        writeSystemLog("config LaunchState");
        changeState(std::make_unique<LaunchState>(*this));
        break;
    case State::DROP:
        writeSystemLog("config DropState");
        changeState(std::make_unique<DropState>(*this));
        break;
    case State::ESCAPE:
        writeSystemLog("config EscapeState");
        changeState(std::make_unique<EscapeState>(*this));
        break;
    case State::DETECTION:
        writeSystemLog("config DetectionState");
        changeState(std::make_unique<DetectionState>(*this));
        break;
    case State::RECORDING:
        writeSystemLog("config RecordingState");
        changeState(std::make_unique<RecordingState>(*this));
        break;
    case State::EXPLORE:
        writeSystemLog("config ExploreState");
        changeState(std::make_unique<ExploreState>(*this));
        break;
    case State::HELPING:
        writeSystemLog("config HelpingState");
        changeState(std::make_unique<HelpingState>(*this));
        break;
    default:
        writeSystemLog("Unknown state");
        writeSystemLog("config CalibrationState");
        changeState(std::make_unique<CalibrationState>(*this));
        break;
    }
}

void CansatController::appendSensorLog() {
    // 1) まず全センサ値をスナップショット
    unsigned long t_ms = millis();

    char date[32];           // 例: "2025-08-23 12:34:56"
    date[0] = '\0';
    // getCurrentDate() が const char* を返すタイプなら strcpy_safety
    {
        const char* p = _gnss.getCurrentDate();   // 不安定なポインタ
        if (p) {
            // 安全コピー（必ず終端する）
            strncpy(date, p, sizeof(date) - 1);
            date[sizeof(date) - 1] = '\0';
        } else {
            strcpy(date, ""); // 空にしておく
        }
    }

    int state_i = _state ? _state->getState() : -1;

    float lat = _gnss.getLatitude();
    float lon = _gnss.getLongitude();
    float alt = _gnss.getAltitude();

    int   cds  = _cds.read();

    float ax = _imu.getAccX(),  ay = _imu.getAccY(),  az = _imu.getAccZ();
    float gx = _imu.getGyroX(), gy = _imu.getGyroY(), gz = _imu.getGyroZ();
    float mx = _imu.getMagX(),  my = _imu.getMagY(),  mz = _imu.getMagZ();
    float roll = _imu.getRoll(), pitch = _imu.getPitch(), heading = _imu.getHeading();

    int voltage_mV = _power.getVoltage(); // フォーマットに合わせて型確認

    // 2) 行を組み立て（行バッファは十分大きく）
    char line[192];
    int len = snprintf(
        line, sizeof(line),
        "%lu,%s,%d,%.6f,%.6f,%.2f,%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.2f,%.2f,%.2f,%d\n",
        t_ms, date, state_i,
        lat, lon, alt,
        cds, ax, ay, az, gx, gy, gz, mx, my, mz, roll, pitch, heading, voltage_mV
    );

    // 3) フォーマット失敗/切り詰め検出
    if (len < 0) {
        _writer.log("appendSensorLog: snprintf failed");
        return;
    }
    if ((size_t)len >= sizeof(line)) {
        _writer.log("appendSensorLog: line truncated, dropping");
        return;
    }

    // 4) バッファ境界チェック（> にして溢れを確実に回避）
    if (_head + (size_t)len > SENSOR_BUFFER_SIZE) {
        // いま溜まっている分を書き出してから新しい行を入れる
        _logger.appendSensorLog(_sensorBuffer, _head); // バイナリ長書き出しAPIであること
        _head = 0;
    }

    // 5) 追記
    memcpy(&_sensorBuffer[_head], line, (size_t)len);
    _head += (size_t)len;
}