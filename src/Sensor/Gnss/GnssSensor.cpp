#include "Sensor/Gnss/GnssSensor.hpp"
#include <Arduino.h>

// 静的インスタンスポインタの初期化
GnssSensor* GnssSensor::_instance = nullptr;

GnssSensor::GnssSensor(int timeout_ms) 
    : _gnss(), _latitude(0), _longitude(0), _altitude(0), _currentDate(""), _posFix(false), _timeout_ms(timeout_ms),
      _dataReady(false), _interruptModeEnabled(false), _timerInterruptEnabled(false), _lastUpdateTime(0), _dataReadyCallback(nullptr) {
    // 静的インスタンスポインタを設定
    _instance = this;
}

GnssSensor::~GnssSensor() {
    // 割り込みモードを無効化
    disableInterruptMode();
    _instance = nullptr;
}

bool GnssSensor::begin() {
	if (_gnss.begin() != 0) {
		return false;
	} else {
		_gnss.select(GPS);
		_gnss.select(QZ_L1CA);
		_gnss.select(QZ_L1S);
	}

	if (_gnss.start(COLD_START) != 0) {
		return false;
	}

 	return true;
}

bool GnssSensor::waitReceive() {
	unsigned long startTime = millis();
	// タイムアウトを5分(300秒)に設定
	const unsigned long timeout = 300000; 

	while (!isPosFix()) {
		update(); // 内部でwaitUpdate(-1)が呼ばれ、データ更新までブロックされる
		if (millis() - startTime > timeout) {
			return false;
		}
	}

	// 測位が完了したので、最新のデータを取得しておく
	update();

	return true;
}

bool GnssSensor::update() {
    static bool posFixFlag = false;

    // 非ブロッキングでデータ更新をチェック（タイムアウト0で即座に戻る）
    if (!_gnss.waitUpdate(0)) {
        // データが更新されていない場合はfalseを返す（ブロッキングしない）
        return false;
    }

    SpNavData navData;
    _gnss.getNavData(&navData);

    bool ledSat = ((navData.posDataExist) && (navData.posFixMode != FixInvalid));
    if (posFixFlag != ledSat) {
        _posFix = ledSat;
        posFixFlag = ledSat;
    }

    if (navData.posDataExist != 0) {
        sprintf(_currentDate, "%04d/%02d/%02d %02d:%02d:%02dZ", 
            navData.time.year, navData.time.month, navData.time.day,
            navData.time.hour, navData.time.minute, navData.time.sec);
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
	return _currentDate;
}

bool GnssSensor::isPosFix() const {
	return _posFix;
}

// 割り込みベースの非同期処理の実装
void GnssSensor::enableInterruptMode() {
    if (!_interruptModeEnabled) {
        _interruptModeEnabled = true;
        _dataReady = false;
        
        // GNSSの割り込み通知を有効化
        // Spresense SDKのioctl(CXD56_GNSS_IOCTL_SIGNAL_SET)に相当
        // Arduino環境では、タイマー割り込みを使用してGNSSデータを定期的にチェック
        // 実際のハードウェア割り込みが利用可能な場合は、ここで設定
        
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
        
        // コールバック関数が設定されている場合は実行
        if (_dataReadyCallback) {
            _dataReadyCallback();
        }
    }
}

// 静的割り込みハンドラー
void GnssSensor::gnssInterruptHandler() {
    if (_instance) {
        _instance->onGnssDataReceived();
    }
}

// タイマー割り込みベースの処理
void GnssSensor::enableTimerInterrupt(unsigned long interval_ms) {
    if (!_timerInterruptEnabled) {
        _timerInterruptEnabled = true;
        _lastUpdateTime = millis();
        
        // Arduino環境でのタイマー割り込み設定
        // 注意: 実際の実装では、使用するArduinoボードに応じて適切なタイマーライブラリを使用
        // 例: TimerOne, TimerThree, ESP32のタイマーなど
        
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
            // データが更新された場合、割り込み通知を送信
            onGnssDataReceived();
        }
    }
}