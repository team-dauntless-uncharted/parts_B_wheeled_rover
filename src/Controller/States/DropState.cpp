#include "DropState.hpp"
#include "NavigationState.hpp"
#include "Controller/CansatController.hpp"

void DropState::onEnter() {
	_ctx.getSerialWriter().log("Entering DropState");
  _startTime = millis();

  _ctx.getSerialWriter().log("CansatController: Initializing Twelite...");
  if (!_ctx.getTwelite().begin()) {
      _ctx.getSerialWriter().log("CansatController: Twelite initialization failed!");
  } else {
      _ctx.getSerialWriter().log("CansatController: Twelite initialized successfully");
  }
}

void DropState::onUpdate() {
	_ctx.getSerialWriter().log("Updating DropState");

  unsigned long elapsedTime = _ctx.getCurrentTime() - _startTime;

  _ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);
  
  // 30秒経過したらLandingStateに移行する
  if (elapsedTime > 30000) {
    _ctx.getSerialWriter().log("30sec elapsed");
    _ctx.changeState(std::make_unique<NavigationState>(_ctx));
  }

  String packet = _ctx.getTwelite().receive();
  if (packet != "") {
    if (packet == "H") {
      _ctx.changeState(std::make_unique<NavigationState>(_ctx));
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