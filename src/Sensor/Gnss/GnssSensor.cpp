#include "GnssSensor.hpp"
#include <Arduino.h>

GnssSensor::GnssSensor() : _gnss(), _latitude(0), _longitude(0), _altitude(0), _currentDate(""), _posFix(false) {}

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

void GnssSensor::update() {
	static bool posFixFlag = false;

	if (_gnss.waitUpdate(-1)) {
		SpNavData navData;
		_gnss.getNavData(&navData);

		bool ledSat = ((navData.posDataExist) && (navData.posFixMode != FixInvalid));
		if (posFixFlag != ledSat) {
			_posFix = ledSat;
			posFixFlag = ledSat;
		}

		if (navData.posDataExist == 0) {
			// No position data
		} else {
			sprintf(_currentDate, "%04d/%02d/%02d %02d:%02d:%02dZ", 
				navData.time.year, navData.time.month, navData.time.day,
				navData.time.hour, navData.time.minute, navData.time.sec);
			_latitude = navData.latitude;
			_longitude = navData.longitude;
			_altitude = navData.altitude;
		}
	}
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
