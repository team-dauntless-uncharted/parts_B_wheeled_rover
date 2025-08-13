#pragma once
#include <Arduino.h>

class TweliteController {
public:
	TweliteController();
	bool begin(unsigned long baud = 115200);

	void send(const char* message);

	int available();
	String receive();

};