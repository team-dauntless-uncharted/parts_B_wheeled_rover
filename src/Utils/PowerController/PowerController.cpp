#include "PowerController.hpp"

PowerController::PowerController() {}

void PowerController::begin() {
	LowPower.begin();
}

int PowerController::getVoltage() {
	return LowPower.getVoltage();
}