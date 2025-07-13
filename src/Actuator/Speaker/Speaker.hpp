#pragma once

class Speaker {
public:
	Speaker(int pin);
	void beep(int freq, int duration);

private:
	int _pin;
};