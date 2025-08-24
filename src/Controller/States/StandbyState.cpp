#include "Controller/States/StandbyState.hpp"
#include "Controller/States/LaunchState.hpp"
#include "Controller/CansatController.hpp"

void StandbyState::onEnter() {
	_ctx.writeSystemLog("Entering StandbyState");

	_startTime = millis();
}

void StandbyState::onUpdate() {
	_ctx.getSerialWriter().log("Updating StandbyState");

	if (_ctx.getGnss().getAltitude() > _ctx.getUserConfig().standbyStateAltThreshold) {
		_ctx.writeSystemLog("StandbyState: Above an altitude. Change to LaunchState");
		_ctx.changeState(std::make_unique<LaunchState>(_ctx));
		return;
	}

    long elapsedTime = millis() - _startTime;
	_ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);
    if (elapsedTime > _ctx.getUserConfig().standbyStateTimeoutThreshold) {
		_ctx.writeSystemLog("StandbyState: Timeout. Change to LaunchState");
		_ctx.changeState(std::make_unique<LaunchState>(_ctx));
		return;
    }
}

void StandbyState::onExit() {
	_ctx.writeSystemLog("Exiting StandbyState");

	if (!_ctx.getSDLogger().writeState((int)State::LAUNCH)) {
		_ctx.writeSystemLog("Failed to write state");
	}

#ifdef USE_FLASH
	if (!_ctx.getFlashIO().writeState((int)State::LAUNCH)) {
		_ctx.writeSystemLog("Failed to write state");
	}
#endif // USE_FLASH
}

State StandbyState::getState() const {
	return State::STANDBY;
}