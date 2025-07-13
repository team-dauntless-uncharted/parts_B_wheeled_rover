#include <Arduino.h>
#include "CdSSensor.hpp"

CdSSensor::CdSSensor(int pin) : _pin(pin) {
    // ピン初期化など必要ならここで
}

int CdSSensor::read() {
	return analogRead(_pin);
}