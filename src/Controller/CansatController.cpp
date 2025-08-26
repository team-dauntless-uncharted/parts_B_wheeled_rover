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
    : _motorR_pins{8, 4, 5},
      _motorL_pins{7, 2, 3},
      _gnss(1000),
      _bno055(),
      _cds(A0),
      _motor(_motorR_pins, _motorL_pins),
      _led{Led(LED0), Led(LED1), Led(LED2), Led(LED3)},
      _speaker(9),
      _heater(6),
      _twelite(11),
      _sdLogger(),
      _camera(),
      _power()
{
}

void CansatController::writeSystemLog(const char* message) {
    _sdLogger.appendSystemLog(message);
    _writer.log(message);
}

void CansatController::begin() {
    _writer.begin();
    _writer.log("CansatController: Starting begin()");

    _twelite.begin(Serial2, 115200);
    _twelite.off();
    setIsConnectTwelite(false);
    
    if (!_sdLogger.begin(CSV_HEADER)) {
        _writer.log("CansatController: SD Logger initialization failed!");
    } else {
        _writer.log("CansatController: SD Logger initialized successfully");
    }
    _sdLogger.appendSystemLog("CansatController: begin() started");

    _power.begin();
    writeSystemLog(_power.getBootCause());

    if (!_gnss.begin()) {
        writeSystemLog("CansatController: GNSS initialization failed!");
    } else {
        writeSystemLog("CansatController: GNSS initialized successfully");
    }

#ifdef WAIT_GNSS_RECEIVE
    if (!_gnss.waitReceive()) {
        writeSystemLog("CansatController: GNSS position fix failed!");
    } else {
        writeSystemLog("CansatController: GNSS position fix succeeded");
    }
#endif
    
    if (!_bno055.begin()) {
        writeSystemLog("CansatController: BNO055 initialization failed!");
    } else {
        writeSystemLog("CansatController: BNO055 initialized successfully");
    }

    configState();
}

void CansatController::update() {
    _gnss.update();
    _acceleration = _bno055.getAcceleration();
    _gyro = _bno055.getGyroscope();
    _magnetic = _bno055.getMagnetometer();
    _euler = _bno055.getEulerAngles();

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
    if (!_sdLogger.readState((int&)state)) {
#ifdef USE_FLASH
        _writer.log("Use Flash");
        _flash.readState((int&)state);
#endif // USE_FLASH
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

    float ax = _acceleration.x, ay = _acceleration.y, az = _acceleration.z;
    float gx = _gyro.x, gy = _gyro.y, gz = _gyro.z;
    float mx = _magnetic.x, my = _magnetic.y, mz = _magnetic.z;
    float roll = _euler.roll, pitch = _euler.pitch, heading = _euler.heading;

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
        _sdLogger.appendSensorLog(_sensorBuffer, _head); // バイナリ長書き出しAPIであること
        _head = 0;
    }

    // 5) 追記
    memcpy(&_sensorBuffer[_head], line, (size_t)len);
    _head += (size_t)len;
}

void CansatController::setLed(const int state) {
    for (int i = 0; i < 4; i++) {
        if (state & (1 << i)) {
            _led[i].on();
        } else {
            _led[i].off();
        }
    }
}