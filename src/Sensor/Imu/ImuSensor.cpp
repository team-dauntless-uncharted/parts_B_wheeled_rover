#include "Sensor/Imu/ImuSensor.hpp"

ImuSensor::ImuSensor() : _accX(0), _accY(0), _accZ(0), _gyroX(0), _gyroY(0), _gyroZ(0), _magX(0), _magY(0), _magZ(0), _heading(0), _roll(0), _pitch(0) {}

bool ImuSensor::begin() {
	return _bno.begin();
}

bool ImuSensor::update() {
	sensors_event_t event;

	// 加速度
	_bno.getEvent(&event, Adafruit_BNO055::VECTOR_ACCELEROMETER);
	_accX = event.acceleration.x;
	_accY = event.acceleration.y;
	_accZ = event.acceleration.z;

	// 加速度がすべて0ならメンバを0にしてfalseを返す
    if (_accX == 0.0f && _accY == 0.0f && _accZ == 0.0f) {
        resetAllValues();
        return false;
    }

	// ジャイロ
	_bno.getEvent(&event, Adafruit_BNO055::VECTOR_GYROSCOPE);
	_gyroX = event.gyro.x;
	_gyroY = event.gyro.y;
	_gyroZ = event.gyro.z;

	if (_gyroX == 0.0f && _gyroY == 0.0f && _gyroZ == 0.0f) {
		resetAllValues();
		return false;
	}

	// 磁力
	_bno.getEvent(&event, Adafruit_BNO055::VECTOR_MAGNETOMETER);
	_magX = event.magnetic.x;
	_magY = event.magnetic.y;
	_magZ = event.magnetic.z;

	if (_magX == 0.0f && _magY == 0.0f && _magZ == 0.0f) {
		resetAllValues();
		return false;
	}

	// オイラー角
	_bno.getEvent(&event, Adafruit_BNO055::VECTOR_EULER);
	_heading = event.orientation.x;
	_pitch   = event.orientation.y;
	_roll    = event.orientation.z;

	if (_heading == 0.0f && _pitch == 0.0f && _roll == 0.0f) {
		resetAllValues();
		return false;
	}

	return true;
}

float ImuSensor::getAccX() const {
	return _accX;
}

float ImuSensor::getAccY() const {
	return _accY;
}

float ImuSensor::getAccZ() const {
	return _accZ;
}

float ImuSensor::getGyroX() const {
	return _gyroX;
}

float ImuSensor::getGyroY() const {
	return _gyroY;
}

float ImuSensor::getGyroZ() const {
	return _gyroZ;
}

float ImuSensor::getMagX() const {
	return _magX;
}

float ImuSensor::getMagY() const {
	return _magY;
}

float ImuSensor::getMagZ() const {
	return _magZ;
}

float ImuSensor::getHeading() const {
	return _heading;
}

float ImuSensor::getRoll() const {
	return _roll;
}

float ImuSensor::getPitch() const {
	return _pitch;
}

void ImuSensor::resetAllValues() {
	_accX = 0.0f;
	_accY = 0.0f;
	_accZ = 0.0f;
	_gyroX = 0.0f;
	_gyroY = 0.0f;
	_gyroZ = 0.0f;
	_magX = 0.0f;
	_magY = 0.0f;
	_magZ = 0.0f;
	_heading = 0.0f;
	_roll = 0.0f;
	_pitch = 0.0f;
}