#include "Controller/States/DropState.hpp"
#include "Controller/States/RecordingState.hpp"
#include "Controller/CansatController.hpp"

void DropState::onEnter() {
  _ctx.writeSystemLog("Entering DropState");

  _ctx.writeSystemLog("CansatController: Twelite initialization started");
  _ctx.getTwelite().begin();
}

void DropState::onUpdate() {
	_ctx.getSerialWriter().log("Updating DropState");

    // 着地を検知したら LANDING モードに遷移する
    // 加速度センサのxyz軸の平方和を計算
    double acc = _ctx.getAcceleration();

    // しきい値以下になったら着地と判断する
    if (acc < _ctx.userConfig.accThreshold) {
		  _ctx.setAccFlag(true);
      _ctx.writeSystemLog("Detect landing");
    }

    if (_ctx.getAccFlag()) {
      _ctx.writeSystemLog("Changing to LandingState");
		  _ctx.changeState(std::make_unique<RecordingState>(_ctx));
    }

}

void DropState::onExit() {
  _ctx.writeSystemLog("Exiting DropState");
}

State DropState::getState() const {
	return State::DROP;
}