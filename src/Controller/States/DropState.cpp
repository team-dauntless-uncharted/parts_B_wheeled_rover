#include "Controller/States/DropState.hpp"
#include "Controller/States/EscapeState.hpp"
#include "Controller/CansatController.hpp"

void DropState::onEnter() {
  _ctx.writeSystemLog("Entering DropState");

  _ctx.writeSystemLog("CansatController: Twelite initialization started");
  if (!_ctx.isInitTwelite()) {
    _ctx.getTwelite().begin(Serial2, 115200);
    _ctx.setInitTwelite(true);
  }

  _startTime = millis();
}

void DropState::onUpdate() {
	_ctx.getSerialWriter().log("Updating DropState");

  unsigned long elapsedTime = millis() - _startTime;

  _ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);
  
  if (elapsedTime > _ctx.getUserConfig().dropStateTimeThreshold) {
    _ctx.getSerialWriter().log("30sec elapsed");
    _ctx.changeState(std::make_unique<EscapeState>(_ctx));
    return;
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

  if (!_ctx.getSDLogger().writeState((int)State::ESCAPE)) {
		_ctx.writeSystemLog("Failed to write state");
	}

#ifdef USE_FLASH
  if (!_ctx.getFlashIO().writeState((int)State::ESCAPE)) {
    _ctx.writeSystemLog("Failed to write state");
  }
#endif // USE_FLASH
}

State DropState::getState() const {
	return State::DROP;
}