#include "Controller/States/LaunchState.hpp"
#include "Controller/States/DropState.hpp"
#include "Controller/CansatController.hpp"

void LaunchState::onEnter() {
	_ctx.writeSystemLog("Entering LaunchState");

	_startTime = millis();
}

void LaunchState::onUpdate() {
	_ctx.getSerialWriter().log("Updating LaunchState");

	// 放出を検知したら DROP モードに遷移する
    if (_ctx.getCds().read() < _ctx.getUserConfig().launchStateCdsThreshold) {
		_ctx.writeSystemLog("Detect separation. Change to DropState");
		_ctx.changeState(std::make_unique<DropState>(_ctx));
		return;
    }

	unsigned long elapsedTime = millis() - _startTime;
	_ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);
	if (elapsedTime > _ctx.getUserConfig().launchStateTimeoutThreshold) {
		_ctx.writeSystemLog("Timeout. Change to DropState");
		_ctx.changeState(std::make_unique<DropState>(_ctx));
		return;
	}
}

void LaunchState::onExit() {
	_ctx.writeSystemLog("Exiting LaunchState");

	if (!_ctx.getSDLogger().writeState((int)State::DROP)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	if (!_ctx.getFlashIO().writeState((int)State::DROP)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
	}
#endif // USE_FLASH
}

State LaunchState::getState() const {
	return State::LAUNCH;
}