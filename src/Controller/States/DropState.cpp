#include "Controller/States/DropState.hpp"
#include "Controller/States/NavigationState.hpp"
#include "Controller/CansatController.hpp"

void DropState::onEnter() {
	_ctx.getSerialWriter().log("Entering DropState");

  _ctx.getTwelite().begin(Serial2, 115200);

  _startTime = _ctx.getCurrentTime();
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

  twelite::Packet pkt;
  if (_ctx.getTwelite().receivePacket(pkt)) {
    if (twelite::TwelitePacket::match(pkt, twelite::C_PARTS, twelite::BROADCAST, twelite::DeployComplete)) {
      _ctx.getSerialWriter().log("DeployComplete received");
      _ctx.changeState(std::make_unique<NavigationState>(_ctx));
    }
  }
}

void DropState::onExit() {
	_ctx.getSerialWriter().log("Exiting DropState");
}

State DropState::getState() const {
	return State::DROP;
}