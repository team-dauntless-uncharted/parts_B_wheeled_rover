#include "DropState.hpp"
#include "LandingState.hpp"
#include "Controller/CansatController.hpp"

void DropState::onEnter() {
	_ctx.getSerialWriter().log("Entering DropState");
  _startTime = millis();
}

void DropState::onUpdate() {
	_ctx.getSerialWriter().log("Updating DropState");

  unsigned long elapsedTime = _ctx.getCurrentTime() - _startTime;

  _ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);
  
  // 30秒経過したらLandingStateに移行する
  if (elapsedTime > 30000) {
    _ctx.getSerialWriter().log("30sec elapsed");
    _ctx.changeState(std::make_unique<LandingState>(_ctx));
  }

  String packet = _ctx.getTwelite().receive();
  if (packet != "") {
    if (packet == "H") {
      _ctx.getSpeaker().playBeep();
      _ctx.changeState(std::make_unique<LandingState>(_ctx));
    }
  }

  // 着地を検知したら LANDING モードに遷移する
  // 加速度センサのxyz軸の平方和を計算
  // double acc = _ctx.getAcceleration();

  // しきい値以下になったら着地と判断する
  // if (acc < _ctx.userConfig.accThreshold) {
	// 	_ctx.setAccFlag(true);
  // }

  // if (_ctx.getAccFlag()) {
	// 	_ctx.changeState(std::make_unique<LandingState>(_ctx));
  // }
}

void DropState::onExit() {
	_ctx.getSerialWriter().log("Exiting DropState");
}

State DropState::getState() const {
	return State::DROP;
}