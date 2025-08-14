#pragma once
#include <Arduino.h>

class TweliteController {
public:
	TweliteController();
	bool begin(unsigned long baud = 115200);

	void send(const char* message);
	void sendf(const char* fmt, ...);

	int available();
	String receive();

	void configMode();

private:
	void sendTweliteCommand(const char *cmd);
};