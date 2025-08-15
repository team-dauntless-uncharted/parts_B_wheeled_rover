#include "Actuator/Heater/Heater.hpp"
#include <Arduino.h>

Heater::Heater(int pin) : _pin(pin) {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void Heater::heat(int pwm, int duration_ms) {
    analogWrite(_pin, pwm);
    delay(duration_ms);
    analogWrite(_pin, 0);
}