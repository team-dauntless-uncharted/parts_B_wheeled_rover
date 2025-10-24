/**
 * @file CansatController.cpp
 * @brief CanSat中央コントローラーの実装
 */

// NOTE: ArduinoJsonのPROGMEM機能を無効化
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
    : _motorR_pins{8, 4, 5},        // 右モーター: PWMピン8, 方向ピン4/5
      _motorL_pins{7, 2, 3},        // 左モーター: PWMピン7, 方向ピン2/3
      _gnss(1000),                  // GNSSタイムアウト1000ms
      _bno055(),
      _cds(A0),                     // CdSセンサー: アナログピンA0
      _motor(_motorR_pins, _motorL_pins),
      _led{Led(LED0), Led(LED1), Led(LED2), Led(LED3)},  // Spresense内蔵LED
      _speaker(9),                  // スピーカー: ピン9
      _sdLogger(),
      _camera(),
      _power()
{
}

void CansatController::writeSystemLog(const char* format, ...) {
    char buf[256];

    // 可変長引数をフォーマット文字列に従って展開
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    // SDカードとシリアル出力の両方に書き込み
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

    // ハートビートログ: 10秒ごとに生存確認ログを出力
    if (now - _lastHeartBeat >= 10000) {
        writeSystemLog("%lu: HeartBeat", now);
        _lastHeartBeat = now;
    }

    // 全センサーの値を更新
    _gnss.update();
    _acceleration = _bno055.getAcceleration();
    _gyro = _bno055.getGyroscope();
    _magnetic = _bno055.getMagnetometer();
    _euler = _bno055.getEulerAngles();

    // センサーログをバッファに追記
    appendSensorLog();

    // 現在の状態の onUpdate() を呼び出し
    if (_state) _state->onUpdate();
}

void CansatController::changeState(std::unique_ptr<ICansatState> newState) {
    // 現在の状態の終了処理
    if (_state) _state->onExit();

    // 状態ポインタを移動（所有権の移譲）
    _state = std::move(newState);

    // 新しい状態の初期化処理
    if (_state) _state->onEnter();
}

void CansatController::readConfigFile() {
    char buffer[512];
    size_t bytesRead = _sdLogger.readJSONFile("/mnt/sd0/config.json", buffer, sizeof(buffer));

    // ファイルが存在しない、または読み込み失敗の場合はデフォルト値を使用
    if (bytesRead == 0) {
        return;
    }

    // ArduinoJsonでJSONをパース
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, buffer);

    // JSONパースに失敗した場合はデフォルト値を使用
    if (error) {
        return;
    }

    // 各キーの存在をチェックしてから値を代入（存在しないキーはデフォルト値を維持）
    
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

    // SDカードから状態番号を読み取り
    if (!_sdLogger.readState((int&)state)) {
        // SDカードから読み取れない場合、Flashから読み取りを試みる
#ifdef USE_FLASH
        _writer.log("Use Flash");
        _flash.readState((int&)state);
#endif // USE_FLASH
    }

    // 状態番号に応じた状態インスタンスを生成
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
    // ステップ1: 全センサー値のスナップショットを取得
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

    int voltage_mV = _power.getVoltage();

    // ステップ2: CSV行を組み立て
    char line[192];
    int len = snprintf(
        line, sizeof(line),
        "%lu,%s,%d,%.6f,%.6f,%.2f,%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.2f,%.2f,%.2f,%d\n",
        t_ms, date, state_i,
        lat, lon, alt,
        cds, ax, ay, az, gx, gy, gz, mx, my, mz, roll, pitch, heading, voltage_mV
    );

    // ステップ3: フォーマット失敗または切り詰めを検出
    if (len < 0) {
        _writer.log("appendSensorLog: snprintf failed");
        return;
    }
    if ((size_t)len >= sizeof(line)) {
        _writer.log("appendSensorLog: line truncated, dropping");
        return;
    }

    // ステップ4: バッファ境界チェック
    if (_head + (size_t)len > SENSOR_BUFFER_SIZE) {
        // バッファが満杯になったら、現在の内容をSDカードに書き出し
        _sdLogger.appendSensorLog(_sensorBuffer, _head);
        _head = 0;  // バッファをリセット
    }

    // ステップ5: バッファに追記
    memcpy(&_sensorBuffer[_head], line, (size_t)len);
    _head += (size_t)len;
}

void CansatController::setLed(const int state) {
    // 各LEDをビットフラグで制御
    // state のビット i が立っていれば LED i を点灯、そうでなければ消灯
    for (int i = 0; i < 4; i++) {
        if (state & (1 << i)) {
            _led[i].on();
        } else {
            _led[i].off();
        }
    }
}