#include "Controller/States/CalibrationState.hpp"
#include "Controller/States/StandbyState.hpp"
#include "Controller/CansatController.hpp"

void CalibrationState::onEnter() {
	_ctx.writeSystemLog("Entering CalibrationState");
}

// センサのキャリブレーション
void CalibrationState::onUpdate() {
	_ctx.getSerialWriter().log("Updating CalibrationState");

	_ctx.writeSystemLog("Change to StandbyState");
	_ctx.changeState(std::make_unique<StandbyState>(_ctx));
}

void CalibrationState::onExit() {
	_ctx.writeSystemLog("Exiting CalibrationState");
}

State CalibrationState::getState() const {
	return State::CALIBRATION;
}