#include "CalibrationState.hpp"
#include "StandbyState.hpp"
#include "Controller/CansatController.hpp"

void CalibrationState::onEnter() {
	_ctx.getSerialWriter().log("Entering CalibrationState");
}

// センサのキャリブレーション
void CalibrationState::onUpdate() {
	_ctx.getSerialWriter().log("Updating CalibrationState");

	_ctx.changeState(std::make_unique<StandbyState>(_ctx));
}

void CalibrationState::onExit() {
	_ctx.getSerialWriter().log("Exiting CalibrationState");
}