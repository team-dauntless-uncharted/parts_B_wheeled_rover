#define ARDUINOJSON_ENABLE_PROGMEM 0

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Controller/CansatController.hpp"

#include "Controller/States/CalibrationState.hpp"
#include "Controller/States/StandbyState.hpp"
#include "Controller/States/LaunchState.hpp"
#include "Controller/States/DropState.hpp"
#include "Controller/States/EscapeState.hpp"
#include "Controller/States/DetectionState.hpp"
#include "Controller/States/RecordingState.hpp"
#include "Controller/States/ExploreState.hpp"

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
      _sdLogger(),
      _camera(),
      _power()
{
}

void CansatController::writeSystemLog(const char* format, ...) {
    char buf[256]; // 必要に応じてサイズ調整
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    _sdLogger.appendSystemLog(buf);
    _writer.log(buf);
}

void CansatController::begin() {
    _writer.begin();
    _writer.log("CansatController: Starting begin()");

    if (!_sdLogger.begin(CSV_HEADER)) {
        for (int i = 0; i < 5; i++) {
            _speaker.playBeep();
            delay(100);
        }
        _writer.log("CansatController: SD Logger initialization failed!");
    } else {
        _writer.log("CansatController: SD Logger initialized successfully");
    }
    _sdLogger.appendSystemLog("CansatController: begin() started");

    _power.begin();
    writeSystemLog(_power.getBootCause());

    readConfigFile();   
    dumpConfig();

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
    
    if (!_bno055.begin(true)) {
        writeSystemLog("CansatController: BNO055 initialization failed!");
    } else {
        writeSystemLog("CansatController: BNO055 initialized successfully");
    }
    // _bno055.setAccelerometerTo16G();

    writeSystemLog("CansatController: begin() finished\ntimestamp: log");
    configState();
}

void CansatController::update() {
    unsigned long now = millis();

    if (now - _lastHeartBeat >= 10000) {
        writeSystemLog("%lu: HeartBeat", now);
        _lastHeartBeat = now;
    }

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

void CansatController::readConfigFile() {
    char buffer[512];
    size_t bytesRead = _sdLogger.readJSONFile("/mnt/sd0/config.json", buffer, sizeof(buffer));
    
    // ファイルが存在しない、または読み込み失敗の場合は何もしない
    if (bytesRead == 0) {
        return;
    }
    
    DynamicJsonDocument doc(512);  // バッファサイズを大きめに設定
    DeserializationError error = deserializeJson(doc, buffer);
    
    // JSONパースに失敗した場合は何もしない
    if (error) {
        return;
    }
    
    // 各キーの存在をチェックしてから値を代入
    
    // Calibration設定
    if (doc.containsKey("Calibration") && doc["Calibration"].containsKey("Timeout")) {
        _config.calibrationStateTimeoutThreshold = doc["Calibration"]["Timeout"];
    }
    
    // Standby設定
    if (doc.containsKey("Standby")) {
        if (doc["Standby"].containsKey("Timeout")) {
            _config.standbyStateTimeoutThreshold = doc["Standby"]["Timeout"];
        }
    }
    
    // Launch設定
    if (doc.containsKey("Launch")) {
        if (doc["Launch"].containsKey("CdS")) {
            _config.launchStateCdsThreshold = doc["Launch"]["CdS"];
        }
        if (doc["Launch"].containsKey("Timeout")) {
            _config.launchStateTimeoutThreshold = doc["Launch"]["Timeout"];
        }
    }
    
    // Drop設定
    if (doc.containsKey("Drop") && doc["Drop"].containsKey("Timeout")) {
        _config.dropStateTimeoutThreshold = doc["Drop"]["Timeout"];
    }
    
    // Escape設定
    if (doc.containsKey("Escape")) {
        if (doc["Escape"].containsKey("Timeout")) {
            _config.escapeStateTimeoutThreshold = doc["Escape"]["Timeout"];
        }
    }
    
    // Detection設定
    if (doc.containsKey("Detection") && doc["Detection"].containsKey("MaxFailedCount")) {
        if (doc["Detection"].containsKey("MaxFailedCount")) {
            _config.detectionMaxFailedCount = doc["Detection"]["MaxFailedCount"];
        }
        if (doc["Detection"].containsKey("Turn180Delay")) {
            _config.detectionTurn180delay = doc["Detection"]["Turn180Delay"];
        }
    }
    
    // Recording設定
    if (doc.containsKey("Recording")) {
        if (doc["Recording"].containsKey("Timeout")) {
            _config.recordingTimeoutThreshold = doc["Recording"]["Timeout"];
        }
        if (doc["Recording"].containsKey("Time")) {
            _config.recordingTime = doc["Recording"]["Time"];
        }
    }
}

void CansatController::dumpConfig() {
    writeSystemLog("\n=== Configuration Dump ===");

    char logBuf[128];  // ログメッセージ用バッファ
    // Calibration設定
    snprintf(logBuf, sizeof(logBuf), "[Calibration] Timeout: %d", 
             _config.calibrationStateTimeoutThreshold);
    writeSystemLog(logBuf);    

    // Standby設定
    snprintf(logBuf, sizeof(logBuf), "Timeout: %d", 
             _config.standbyStateTimeoutThreshold);
    writeSystemLog(logBuf);
    
    // Launch設定
    snprintf(logBuf, sizeof(logBuf), "[Launch] CdS: %d, Timeout: %d", 
             _config.launchStateCdsThreshold, _config.launchStateTimeoutThreshold);
    writeSystemLog(logBuf);
    
    // Drop設定
    snprintf(logBuf, sizeof(logBuf), "[Drop] Timeout: %d", 
             _config.dropStateTimeoutThreshold);
    writeSystemLog(logBuf);
    
    // Escape設定
    snprintf(logBuf, sizeof(logBuf), "[Escape] Timeout: %d", 
             _config.escapeStateTimeoutThreshold);
    writeSystemLog(logBuf);
    
    // Detection設定
    snprintf(logBuf, sizeof(logBuf), "[Detection] MaxFailedCount: %d", 
             _config.detectionMaxFailedCount);
    writeSystemLog(logBuf);
    
    // Recording設定
    snprintf(logBuf, sizeof(logBuf), "[Recording] Timeout: %d, Time: %d", 
             _config.recordingTimeoutThreshold, _config.recordingTime);
    writeSystemLog(logBuf);

    writeSystemLog("=== End of Configuration Dump ===\n");
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
        writeSystemLog("%lu: config CalibrationState", millis());
        changeState(std::make_unique<CalibrationState>(*this));
        break;
    case State::STANDBY:
        writeSystemLog("%lu: config StandbyState", millis());
        changeState(std::make_unique<StandbyState>(*this));
        break;
    case State::LAUNCH:
        writeSystemLog("%lu: config LaunchState", millis());
        changeState(std::make_unique<LaunchState>(*this));
        break;
    case State::DROP:
        writeSystemLog("%lu: config DropState", millis());
        changeState(std::make_unique<DropState>(*this));
        break;
    case State::ESCAPE:
        writeSystemLog("%lu: config EscapeState", millis());
        changeState(std::make_unique<EscapeState>(*this));
        break;
    case State::DETECTION:
        writeSystemLog("%lu: config DetectionState", millis());
        changeState(std::make_unique<DetectionState>(*this));
        break;
    case State::RECORDING:
        writeSystemLog("%lu: config RecordingState", millis());
        changeState(std::make_unique<RecordingState>(*this));
        break;
    case State::EXPLORE:
        writeSystemLog("%lu: config ExploreState", millis());
        changeState(std::make_unique<ExploreState>(*this));
        break;
    default:
        writeSystemLog("%lu: config CalibrationState", millis());
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