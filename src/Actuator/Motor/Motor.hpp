#pragma once

class Motor {
public:
	Motor(int pinR[3], int pinL[3]);
	void forward(int pwm);
	void backward(int pwm);
	void turnRight(int pwm);
	void turnLeft(int pwm);
	void stop();

	void rightForward(int pwm);
	void leftForward(int pwm);
	void rightBackward(int pwm);
	void leftBackward(int pwm);
	void rightStop();
	void leftStop();

    void snakeForwardSmooth(int pwm, int duration, int frequency);

private:
	int _motorR[3];
	int _motorL[3];
};