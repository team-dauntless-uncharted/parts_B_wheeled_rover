#include <Arduino.h>
#include "Actuator/Motor/Motor.hpp"

Motor::Motor(int pinR[3], int pinL[3]) {
	for (int i = 0; i < 3; ++i) {
		_motorR[i] = pinR[i];
		_motorL[i] = pinL[i];
		pinMode(_motorR[i], OUTPUT);
		pinMode(_motorL[i], OUTPUT);
	}
}

void Motor::forward(int pwm) {
	digitalWrite(_motorR[0], HIGH);
	digitalWrite(_motorR[1], LOW);
	analogWrite(_motorR[2], pwm);
	digitalWrite(_motorL[0], HIGH);
	digitalWrite(_motorL[1], LOW);
	analogWrite(_motorL[2], pwm);
}

void Motor::backward(int pwm) {
	digitalWrite(_motorR[0], LOW);
	digitalWrite(_motorR[0], HIGH);
	analogWrite(_motorR[2], pwm);
	digitalWrite(_motorL[0], LOW);
	digitalWrite(_motorL[0], HIGH);
	analogWrite(_motorL[2], pwm);
}

void Motor::turnRight(int pwm) {
	digitalWrite(_motorR[0], HIGH);
	digitalWrite(_motorR[1], LOW);
	analogWrite(_motorR[2], pwm);
	digitalWrite(_motorL[0], LOW);
	digitalWrite(_motorL[1], HIGH);
	analogWrite(_motorL[2], pwm);
}

void Motor::turnLeft(int pwm) {
	digitalWrite(_motorR[0], LOW);
	digitalWrite(_motorR[1], HIGH);
	analogWrite(_motorR[2], pwm);
	digitalWrite(_motorL[0], HIGH);
	digitalWrite(_motorL[1], LOW);
	analogWrite(_motorL[2], pwm);
}

void Motor::stop() {
	analogWrite(_motorR[2], 0);
	analogWrite(_motorL[2], 0);
}