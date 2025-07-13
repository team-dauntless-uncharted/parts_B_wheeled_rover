/**
 * @file Led.hpp
 * 
 * 内蔵LEDを制御する
 */

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