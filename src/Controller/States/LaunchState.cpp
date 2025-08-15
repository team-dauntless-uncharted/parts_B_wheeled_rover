#include "Controller/States/LaunchState.hpp"
#include "Controller/States/DropState.hpp"
#include "Controller/CansatController.hpp"

void LaunchState::onEnter() {
	_ctx.writeSystemLog("Entering LaunchState");
}

void LaunchState::onUpdate() {
	_ctx.getSerialWriter().log("Updating LaunchState");

	// 放出を検知したら DROP モードに遷移する
    if (_ctx.getCdsValue() < _ctx.userConfig.cdsThreshold) {
		_ctx.setCdsFlag(true);
		_ctx.writeSystemLog("Detect separation");
    }

    if (_ctx.getCdsFlag()) {
		_ctx.writeSystemLog("Changing to DropState");
		_ctx.changeState(std::make_unique<DropState>(_ctx));
    }
}

void LaunchState::onExit() {
	_ctx.writeSystemLog("Exiting LaunchState");
}

State LaunchState::getState() const {
	return State::LAUNCH;
}