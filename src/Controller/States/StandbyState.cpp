#include "StandbyState.hpp"
#include "LaunchState.hpp"
#include "Controller/CansatController.hpp"

void StandbyState::onEnter() {
	_ctx.getSerialWriter().log("Entering StandbyState");
}

void StandbyState::onUpdate() {
	_ctx.getSerialWriter().log("Updating StandbyState");

	if (_ctx.getCurrentAlt() > _ctx.userConfig.altThreshold) {
		_ctx.setAltFlag(true);
	}

    long elapsedTime = millis() - _ctx.getCurrentTime();
    if (elapsedTime > _ctx.userConfig.timeThreshold) {
		_ctx.setTimeFlag(true);
    }

    // 高度または時間の条件を満たしたらモード変更
    if (_ctx.getAltFlag() || _ctx.getTimeFlag()) {
		_ctx.changeState(std::make_unique<LaunchState>(_ctx));
    }
}

void StandbyState::onExit() {
	_ctx.getSerialWriter().log("Exiting StandbyState");
}

State StandbyState::getState() const {
	return State::STANDBY;
}