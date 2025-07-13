#include <Arduino.h>
#include "Speaker.hpp"

Speaker::Speaker(int pin) : _pin(pin) {
	pinMode(_pin, OUTPUT);
}

void Speaker::beep(int freq, int duration) {
	tone(_pin, freq, duration);
}