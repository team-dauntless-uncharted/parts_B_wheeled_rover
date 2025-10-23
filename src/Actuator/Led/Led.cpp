/**
 * @file Led.cpp
 * @brief LED制御クラスの実装
 */

#include <Arduino.h>
#include "Actuator/Led/Led.hpp"

Led::Led(int pin) : _pin(pin), _state(false) {
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
}

void Led::on() {
	digitalWrite(_pin, HIGH);
	_state = true;
}

void Led::off() {
	digitalWrite(_pin, LOW);
	_state = false;
}

void Led::toggle() {
	_state = !_state;
	digitalWrite(_pin, _state ? HIGH : LOW);
}