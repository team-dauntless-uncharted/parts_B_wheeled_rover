#include "GnssSensor.hpp"

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
			_currentDate = createDate(navData.time);
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

String GnssSensor::getCurrentDate() const {
	return _currentDate;
}

bool GnssSensor::isPosFix() const {
	return _posFix;
}

String GnssSensor::createDate(SpGnssTime time) {
	// YYYY/MM/DD hh:mm:ssZ
	char buffer[STRING_BUFFER_SIZE];
	sprintf(buffer, "%04d/%02d/%02d %02d:%02d:%02dZ", time.year, time.month, time.day, time.hour, time.minute, time.sec);
	return String(buffer);
}