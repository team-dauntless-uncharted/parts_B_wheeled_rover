#include "Sensor/Imu/ImuSensor.hpp"

ImuSensor::ImuSensor() : _accX(0), _accY(0), _accZ(0), _gyroX(0), _gyroY(0), _gyroZ(0), _magX(0), _magY(0), _magZ(0), _heading(0), _roll(0), _pitch(0) {}

bool ImuSensor::begin() {
	return _bno.begin();
}

bool ImuSensor::update() {
	sensors_event_t acc_event;
	sensors_event_t gyro_event;
	sensors_event_t mag_event;
	sensors_event_t euler_event;

	_bno.getEvent(&acc_event, Adafruit_BNO055::VECTOR_ACCELEROMETER);
	_bno.getEvent(&gyro_event, Adafruit_BNO055::VECTOR_GYROSCOPE);
	_bno.getEvent(&mag_event, Adafruit_BNO055::VECTOR_MAGNETOMETER);
	_bno.getEvent(&euler_event, Adafruit_BNO055::VECTOR_EULER);

	// 加速度
	_accX = acc_event.acceleration.x;
	_accY = acc_event.acceleration.y;
	_accZ = acc_event.acceleration.z;

	// ジャイロ
	_gyroX = gyro_event.gyro.x;
	_gyroY = gyro_event.gyro.y;
	_gyroZ = gyro_event.gyro.z;

	// 磁力
	_magX = mag_event.magnetic.x;
	_magY = mag_event.magnetic.y;
	_magZ = mag_event.magnetic.z;

	// オイラー角
	_heading = euler_event.orientation.x;
	_pitch   = euler_event.orientation.y;
	_roll    = euler_event.orientation.z;

	Serial.printf("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
			_accX, _accY, _accZ, _gyroX, _gyroY, _gyroZ, _magX, _magY, _magZ, _heading, _roll, _pitch);

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