#include "Controller/States/DropState.hpp"
#include "Controller/States/EscapeState.hpp"
#include "Controller/CansatController.hpp"

void DropState::onEnter() {
  _ctx.writeSystemLog("Entering DropState");

  _ctx.writeSystemLog("CansatController: Twelite initialization started");
  _ctx.getTwelite().begin(Serial2, 115200);

  _startTime = _ctx.getCurrentTime();
}

void DropState::onUpdate() {
	_ctx.getSerialWriter().log("Updating DropState");

  unsigned long elapsedTime = _ctx.getCurrentTime() - _startTime;

  _ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);
  
  if (elapsedTime > 30000) {
    _ctx.getSerialWriter().log("30sec elapsed");
    _ctx.changeState(std::make_unique<EscapeState>(_ctx));
  }

  twelite::Packet pkt;
  if (_ctx.getTwelite().receivePacket(pkt)) {
    if (twelite::TwelitePacket::match(pkt, twelite::C_PARTS, twelite::BROADCAST, twelite::DeployComplete)) {
      _ctx.getSerialWriter().log("DeployComplete received");
      _ctx.changeState(std::make_unique<EscapeState>(_ctx));
    }
  }
}

void DropState::onExit() {
  _ctx.writeSystemLog("Exiting DropState");
}

State DropState::getState() const {
	return State::DROP;
}