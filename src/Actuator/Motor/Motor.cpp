#include <Arduino.h>
#include <math.h>
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
	digitalWrite(_motorR[1], HIGH);
	analogWrite(_motorR[2], pwm);
	digitalWrite(_motorL[0], LOW);
	digitalWrite(_motorL[1], HIGH);
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

void Motor::rightForward(int pwm) {
    digitalWrite(_motorR[0], HIGH);
    digitalWrite(_motorR[1], LOW);
    analogWrite(_motorR[2], pwm);
}

void Motor::leftForward(int pwm) {
    digitalWrite(_motorL[0], HIGH);
    digitalWrite(_motorL[1], LOW);
    analogWrite(_motorL[2], pwm);
}

void Motor::rightBackward(int pwm) {
    digitalWrite(_motorR[0], LOW);
    digitalWrite(_motorR[1], HIGH);
    analogWrite(_motorR[2], pwm);
}

void Motor::leftBackward(int pwm) {
    digitalWrite(_motorL[0], LOW);
    digitalWrite(_motorL[1], HIGH);
    analogWrite(_motorL[2], pwm);
}

void Motor::rightStop() {
    analogWrite(_motorR[2], 0);
}

void Motor::leftStop() {
    analogWrite(_motorL[2], 0);
}

void Motor::snakeForwardSmooth(int pwm, int duration, int frequency) {
    unsigned long start = millis();
    while (millis() - start < (unsigned long)duration) {
        float t = (millis() - start) / 1000.0; // 秒に変換
        float offset = sin(2 * M_PI * frequency * t); // -1～1

        int pwmR = pwm * (1.0 - 0.3 * offset); // 右モータ
        int pwmL = pwm * (1.0 + 0.3 * offset); // 左モータ

        analogWrite(_motorR[2], constrain(pwmR, 0, 255));
        analogWrite(_motorL[2], constrain(pwmL, 0, 255));
    }
    stop();
}