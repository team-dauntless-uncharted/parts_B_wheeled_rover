#include "Controller/States/StandbyState.hpp"
#include "Controller/States/LaunchState.hpp"
#include "Controller/CansatController.hpp"

void StandbyState::onEnter() {
	_ctx.writeSystemLog("%lu: Entering StandbyState", millis());

	_ctx.getSpeaker().playState((int)State::STANDBY);
	_ctx.setLed((int)State::STANDBY);

	_startTime = millis();
}

void StandbyState::onUpdate() {
	_ctx.getSerialWriter().log("Updating StandbyState");

    long elapsedTime = millis() - _startTime;
	_ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);
    if (elapsedTime > _ctx.getUserConfig().standbyStateTimeoutThreshold) {
		_ctx.writeSystemLog("%lu: Timeout. Change to LaunchState", millis());
		_ctx.changeState(std::make_unique<LaunchState>(_ctx));
		return;
    }
}

void StandbyState::onExit() {
	_ctx.writeSystemLog("%lu: Exiting StandbyState", millis());

	if (!_ctx.getSDLogger().writeState((int)State::LAUNCH)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	if (!_ctx.getFlashIO().writeState((int)State::LAUNCH)) {
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

State StandbyState::getState() const {
	return State::STANDBY;
}