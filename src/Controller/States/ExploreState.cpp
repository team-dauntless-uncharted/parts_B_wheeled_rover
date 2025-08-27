#include "Controller/States/ExploreState.hpp"
#include "Controller/States/HelpingState.hpp"
#include "Controller/CansatController.hpp"

void ExploreState::onEnter() {
	_ctx.writeSystemLog("%lu: Entering ExploreState", millis());

	// _ctx.getSpeaker().playState((int)State::EXPLORE);
	_ctx.setLed((int)State::EXPLORE);

	if (!setPhotoMode()) {
		_ctx.writeSystemLog("%lu: Failed to set explore mode", millis());
	} else {
		_ctx.writeSystemLog("%lu: Explore mode set", millis());
	}
}

void ExploreState::onUpdate() {
	_ctx.getSerialWriter().log("Updating ExploreState");

	// 適当に移動
	_ctx.getMotor().forward(150);
	delay(random(500, 1500));
	_ctx.getMotor().stop();
	delay(100);	

	if (random(2) == 0) {
		_ctx.getMotor().turnLeft(150);
	} else {
		_ctx.getMotor().turnRight(150);
	}
	delay(random(200, 500));
	_ctx.getMotor().stop();
	delay(100);

	// 画像を撮影する
    void* imgBuff = nullptr;
    size_t imgSize = 0;
    if (_ctx.getCamera().takePicture(&imgBuff, &imgSize)) {
        _ctx.getSerialWriter().log("Save taken picture to SD card...");
        _ctx.getSDLogger().saveJPEGImage(imgBuff, imgSize);
    } else {
		_ctx.getSerialWriter().log("Failed to take picture");
    }

	double latitude = _ctx.getGnss().getLatitude();
	double longitude = _ctx.getGnss().getLongitude();

	if (fabs(latitude - _prevLatitude) < 0.000001 && fabs(longitude - _prevLongitude) < 0.000001) {
		_sameCount++;
	} else {
		_sameCount = 0;
	}

	if (_sameCount >= SAME_LIMIT) {
		// HELPING_STATE
		_ctx.writeSystemLog("%lu: Can't Move. Changing to HELPING_STATE", millis());
		_ctx.changeState(std::make_unique<HelpingState>(_ctx));
		return;
	}

	_prevLatitude = latitude;
	_prevLongitude = longitude;
}

void ExploreState::onExit() {
	_ctx.writeSystemLog("%lu: Exiting ExploreState", millis());
	if (_isInitCamera) {
		endPhotoMode();
	}

	if (!_ctx.getSDLogger().writeState((int)State::HELPING)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	if (!_ctx.getFlashIO().writeState((int)State::HELPING)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
	}
#endif // USE_FLASH
}

State ExploreState::getState() const {
	return State::EXPLORE;
}

bool ExploreState::setPhotoMode() {
	if (!_ctx.getCamera().begin(PHOTO_MODE)) {
		return false;
	}

	if (!_ctx.getCamera().startStreaming(true)) {
		return false;
	}

	_isInitCamera = true;
	return true;
}

void ExploreState::endPhotoMode() {
	_ctx.getCamera().end();
}
