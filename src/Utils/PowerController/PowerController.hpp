#pragma once

#include <LowPower.h>

class PowerController {
public:
	PowerController();

	void begin();

	int getVoltage();

	const char *getBootCause();
};