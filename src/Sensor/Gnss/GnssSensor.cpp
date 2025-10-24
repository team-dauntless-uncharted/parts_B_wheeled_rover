/**
 * @file GnssSensor.cpp
 * @brief GNSSセンサー制御クラスの実装
 */

#include "Sensor/Gnss/GnssSensor.hpp"
#include <Arduino.h>

// 静的インスタンスポインタの初期化
GnssSensor* GnssSensor::_instance = nullptr;

GnssSensor::GnssSensor(int timeout_ms)
    : _gnss(), _latitude(0), _longitude(0), _altitude(0), _currentDate(""), _posFix(false), _timeout_ms(timeout_ms),
      _dataReady(false), _interruptModeEnabled(false), _timerInterruptEnabled(false), _lastUpdateTime(0), _dataReadyCallback(nullptr) {
    // 静的インスタンスポインタを設定（割り込みハンドラーから参照するため）
    _instance = this;
}

GnssSensor::~GnssSensor() {
    disableInterruptMode();
    _instance = nullptr;
}

bool GnssSensor::begin() {
	if (_gnss.begin() != 0) {
		return false;
	}

	// 使用する衛星システムを選択
	_gnss.select(GPS);        // GPS衛星（米国）
	_gnss.select(QZ_L1CA);    // 準天頂衛星 みちびき L1C/A信号
	_gnss.select(QZ_L1S);     // 準天頂衛星 みちびき L1S信号（サブメータ級測位補強）

	// COLD_STARTで測位開始（最初から衛星を探索）
	if (_gnss.start(COLD_START) != 0) {
		return false;
	}

 	return true;
}

bool GnssSensor::waitReceive() {
	unsigned long startTime = millis();
	const unsigned long timeout = 300000;  // タイムアウト: 5分（300秒）

	// 測位完了まで待機（ブロッキング）
	while (!isPosFix()) {
		update();
		if (millis() - startTime > timeout) {
			return false;  // タイムアウト
		}
	}

	// 測位完了後、最新データを取得
	update();

	return true;
}

bool GnssSensor::update() {
    static bool posFixFlag = false;

    // 非ブロッキングでデータ更新をチェック
    // タイムアウト0 = 即座に戻る（データがなければfalse）
    if (!_gnss.waitUpdate(0)) {
        return false;
    }

    // 測位データを取得
    SpNavData navData;
    _gnss.getNavData(&navData);

    // 測位完了フラグの更新
    // posDataExist: 測位データが存在するか
    // posFixMode != FixInvalid: 有効な測位モードか
    bool ledSat = ((navData.posDataExist) && (navData.posFixMode != FixInvalid));
    if (posFixFlag != ledSat) {
        _posFix = ledSat;
        posFixFlag = ledSat;
    }

    // 測位データが存在する場合、内部変数を更新
    if (navData.posDataExist != 0) {
        // UTC時刻を文字列化
        sprintf(_currentDate, "%04d/%02d/%02d %02d:%02d:%02dZ",
            navData.time.year, navData.time.month, navData.time.day,
            navData.time.hour, navData.time.minute, navData.time.sec);

        // 位置情報を更新
        _latitude = navData.latitude;
        _longitude = navData.longitude;
        _altitude = navData.altitude;

        // 割り込みモードが有効な場合、データ受信通知を送信
        if (_interruptModeEnabled) {
            onGnssDataReceived();
        }
    }

    return true;
}

double GnssSensor::getLatitude() const {
	return _latitude;
}

double GnssSensor::getLongitude() const {
	return _longitude;
}

double GnssSensor::getAltitude() const {
	return _altitude;
}

char *GnssSensor::getCurrentDate() const {
	return const_cast<char*>(_currentDate);
}

bool GnssSensor::isPosFix() const {
	return _posFix;
}

// --- 割り込みベースの非同期処理の実装 ---

void GnssSensor::enableInterruptMode() {
    if (!_interruptModeEnabled) {
        _interruptModeEnabled = true;
        _dataReady = false;

        // NOTE: Spresense SDKではioctl(CXD56_GNSS_IOCTL_SIGNAL_SET)で
        // ハードウェア割り込みを設定できるが、Arduino環境では未対応
        // 現在はタイマーベースでエミュレートしている

        Serial.println("GNSS interrupt mode enabled");
    }
}

void GnssSensor::disableInterruptMode() {
    if (_interruptModeEnabled) {
        _interruptModeEnabled = false;
        _dataReady = false;
        
        // GNSSの割り込み通知を無効化
        Serial.println("GNSS interrupt mode disabled");
    }
}

void GnssSensor::setDataReadyCallback(std::function<void()> callback) {
    _dataReadyCallback = callback;
}

bool GnssSensor::isDataReady() const {
    return _dataReady;
}

void GnssSensor::clearDataReadyFlag() {
    _dataReady = false;
}

void GnssSensor::onGnssDataReceived() {
    if (_interruptModeEnabled) {
        _dataReady = true;

        // コールバック関数が設定されていれば実行
        if (_dataReadyCallback) {
            _dataReadyCallback();
        }
    }
}

void GnssSensor::gnssInterruptHandler() {
    // 静的メソッドなので、インスタンスポインタ経由で呼び出す
    if (_instance) {
        _instance->onGnssDataReceived();
    }
}

// --- タイマー割り込みベースの処理 ---

void GnssSensor::enableTimerInterrupt(unsigned long interval_ms) {
    if (!_timerInterruptEnabled) {
        _timerInterruptEnabled = true;
        _lastUpdateTime = millis();

        // NOTE: 実際のタイマー割り込み設定は未実装
        // Spresenseでタイマー割り込みを使用する場合、
        // ボード固有のタイマーライブラリが必要

        Serial.print("GNSS timer interrupt enabled with interval: ");
        Serial.print(interval_ms);
        Serial.println("ms");
    }
}

void GnssSensor::disableTimerInterrupt() {
    if (_timerInterruptEnabled) {
        _timerInterruptEnabled = false;
        Serial.println("GNSS timer interrupt disabled");
    }
}

void GnssSensor::handleTimerInterrupt() {
    if (_timerInterruptEnabled) {
        // 定期的にGNSSデータを更新
        if (update()) {
            // データ更新成功時、割り込み通知を送信
            onGnssDataReceived();
        }
    }
}