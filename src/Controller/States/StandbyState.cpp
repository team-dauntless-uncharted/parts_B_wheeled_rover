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
		_ctx.setAltFlag(true);
		_ctx.writeSystemLog("above a certain altitude");
	}

    long elapsedTime = millis() - _startTime;
    if (elapsedTime > _ctx.getUserConfig().standbyStateTimeThreshold) {
		_ctx.setTimeFlag(true);
		_ctx.writeSystemLog("above a certain time");
		_ctx.changeState(std::make_unique<LaunchState>(_ctx));
		return;
    }

    // 高度または時間の条件を満たしたらモード変更
    if (_ctx.getAltFlag() || _ctx.getTimeFlag()) {
		_ctx.writeSystemLog("Changing to LaunchState");
		_ctx.changeState(std::make_unique<LaunchState>(_ctx));
		return;
    }
}

void StandbyState::onExit() {
	_ctx.writeSystemLog("Exiting StandbyState");

	if (!_ctx.getLogger().writeState(State::LAUNCH)) {
		_ctx.writeSystemLog("Failed to write state");
		// Flash
	}
}

State StandbyState::getState() const {
	return State::STANDBY;
}