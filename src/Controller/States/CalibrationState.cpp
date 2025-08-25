#include "Controller/States/CalibrationState.hpp"
#include "Controller/States/StandbyState.hpp"
#include "Controller/CansatController.hpp"

void CalibrationState::onEnter() {
	_ctx.writeSystemLog("Entering CalibrationState");

	_ctx.getSpeaker().playState((int)State::CALIBRATION);
	_ctx.setLed((int)State::CALIBRATION);
}

// センサのキャリブレーション
void CalibrationState::onUpdate() {
	_ctx.getSerialWriter().log("Updating CalibrationState");

	_ctx.writeSystemLog("Change to StandbyState");
	_ctx.changeState(std::make_unique<StandbyState>(_ctx));
}

void CalibrationState::onExit() {
	_ctx.writeSystemLog("Exiting CalibrationState");

	if (!_ctx.getSDLogger().writeState((int)State::STANDBY)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	if (!_ctx.getFlashIO().writeState((int)State::STANDBY)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
	}
#endif // USE_FLASH
}

State CalibrationState::getState() const {
	return State::CALIBRATION;
}