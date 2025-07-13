#pragma once

class Led {
public:
	Led(int pin);
	void on();
	void off();
	void toggle();

private:
	int _pin;
	bool _state;
};