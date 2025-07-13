#pragma once

#include <Adafruit_BNO055.h>

class ImuSensor {
public:
	ImuSensor();
	bool begin();
	void update();
	float getAccX() const;
	float getAccY() const;
	float getAccZ() const;
	float getGyroX() const;
	float getGyroY() const;
	float getGyroZ() const;
	float getMagX() const;
	float getMagY() const;
	float getMagZ() const;
	float getHeading() const;
	float getRoll() const;
	float getPitch() const;

private:
	Adafruit_BNO055 _bno;
	float _accX;
	float _accY;
	float _accZ;
	float _gyroX;
	float _gyroY;
	float _gyroZ;
	float _magX;
	float _magY;
	float _magZ;
	float _heading;
	float _roll;
	float _pitch;
};