#include "Controller/States/DropState.hpp"
#include "Controller/States/EscapeState.hpp"
#include "Controller/CansatController.hpp"

void DropState::onEnter() {
  _ctx.writeSystemLog("Entering DropState");

	_ctx.getSpeaker().playState((int)State::DROP);
  _ctx.setLed((int)State::DROP);

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
  if (elapsedTime > _ctx.getUserConfig().dropStateTimeoutThreshold) {
    _ctx.writeSystemLog("Timeout. Change to EscapeState");
    _ctx.changeState(std::make_unique<EscapeState>(_ctx));
    return;
  }

  twelite::Packet pkt;
  if (_ctx.getTwelite().receivePacket(pkt)) {
    if (twelite::TwelitePacket::match(pkt, twelite::C_PARTS, twelite::BROADCAST, twelite::DeployComplete)) {
      _ctx.writeSystemLog("DeployComplete received. Change to EscapeState");
      _ctx.changeState(std::make_unique<EscapeState>(_ctx));
      return;
    }
  }
}

void DropState::onExit() {
  _ctx.writeSystemLog("Exiting DropState");

  if (!_ctx.getSDLogger().writeState((int)State::ESCAPE)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
  if (!_ctx.getFlashIO().writeState((int)State::ESCAPE)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
  }
#endif // USE_FLASH

	if (!_ctx.isInitCamera()) {
		if (!_ctx.getCamera().begin(PHOTO_MODE)) {
			return;
		}

		if (!_ctx.getCamera().startStreaming(true)) {
			return;
		}
		_ctx.setInitCamera(true);
	}

	void* imgBuff = nullptr;
    size_t imgSize = 0;
    if (_ctx.getCamera().takePicture(&imgBuff, &imgSize)) {
        _ctx.getSerialWriter().log("Save taken picture to SD card...");
        _ctx.getSDLogger().saveJPEGImage(imgBuff, imgSize);
    } else {
		_ctx.getSerialWriter().log("Failed to take picture");
    }

	_ctx.getCamera().end();
	_ctx.setInitCamera(false);
}

State DropState::getState() const {
	return State::DROP;
}