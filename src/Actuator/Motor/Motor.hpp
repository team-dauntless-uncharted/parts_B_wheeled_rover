#pragma once

class Motor {
public:
	Motor(int pinR[3], int pinL[3]);
	void forward(int pwm);
	void turnRight(int pwm);
	void turnLeft(int pwm);
	void stop();

private:
	int _motorR[3];
	int _motorL[3];
};