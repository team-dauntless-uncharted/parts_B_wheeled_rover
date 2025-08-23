#include "Controller/States/LaunchState.hpp"
#include "Controller/States/DropState.hpp"
#include "Controller/CansatController.hpp"

void LaunchState::onEnter() {
	_ctx.writeSystemLog("Entering LaunchState");
}

void LaunchState::onUpdate() {
	_ctx.getSerialWriter().log("Updating LaunchState");

	// 放出を検知したら DROP モードに遷移する
    if (_ctx.getCds().read() < _ctx.getUserConfig().launchStateCdsThreshold) {
		_ctx.setCdsFlag(true);
		_ctx.writeSystemLog("Detect separation");
    }

    if (_ctx.getCdsFlag()) {
		_ctx.writeSystemLog("Changing to DropState");
		_ctx.changeState(std::make_unique<DropState>(_ctx));
		return;
    }
}

void LaunchState::onExit() {
	_ctx.writeSystemLog("Exiting LaunchState");

	if (!_ctx.getSDLogger().writeState((int)State::DROP)) {
		_ctx.writeSystemLog("Failed to write state");
	}

#ifdef USE_FLASH
	if (!_ctx.getFlashIO().writeState((int)State::DROP)) {
		_ctx.writeSystemLog("Failed to write state");
	}
#endif // USE_FLASH
}

State LaunchState::getState() const {
	return State::LAUNCH;
}