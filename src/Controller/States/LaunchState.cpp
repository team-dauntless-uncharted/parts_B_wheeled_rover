#include "LaunchState.hpp"
#include "DropState.hpp"
#include "Controller/CansatController.hpp"

void LaunchState::onEnter() {
	_ctx.getSerialWriter().log("Entering LaunchState");
}

void LaunchState::onUpdate() {
	_ctx.getSerialWriter().log("Updating LaunchState");

	// 放出を検知したら DROP モードに遷移する
    if (_ctx.getCdsValue() < _ctx.userConfig.cdsThreshold) {
		_ctx.setCdsFlag(true);
    }

    if (_ctx.getCdsFlag()) {
		_ctx.changeState(std::make_unique<DropState>(_ctx));
    }
}

void LaunchState::onExit() {
	_ctx.getSerialWriter().log("Exiting LaunchState");
}