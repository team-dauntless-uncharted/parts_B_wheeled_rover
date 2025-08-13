#pragma once
#include <Arduino.h>

class SerialWriter {
public:
	SerialWriter();
	bool begin(unsigned long baud = 115200);

	void log(const char* message);
	void logf(const char* fmt, ...);
};