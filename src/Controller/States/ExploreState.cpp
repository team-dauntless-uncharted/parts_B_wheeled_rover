#include "Controller/States/ExploreState.hpp"
#include "Controller/States/HelpingState.hpp"
#include "Controller/CansatController.hpp"

void ExploreState::onEnter() {
	_ctx.writeSystemLog("Entering ExploreState");

	if (!setExploreMode()) {
		_ctx.writeSystemLog("Failed to set explore mode");
	} else {
		_ctx.writeSystemLog("Explore mode set");
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
        _ctx.getLogger().saveJPEGImage(imgBuff, imgSize);
    } else {
        _ctx.writeSystemLog("Failed to take picture");
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
		_ctx.writeSystemLog("Changing to HELPING_STATE");
		_ctx.changeState(std::make_unique<HelpingState>(_ctx));
		return;
	}

	_prevLatitude = latitude;
	_prevLongitude = longitude;
}

void ExploreState::onExit() {
	_ctx.writeSystemLog("Exiting ExploreState");
	if (_isInitCamera) {
		endExploreMode();
	}

	if (!_ctx.getLogger().writeState(State::HELPING)) {
		_ctx.writeSystemLog("Failed to write state");
		// Flash
	}
}

State ExploreState::getState() const {
	return State::EXPLORE;
}

bool ExploreState::setExploreMode() {
	if (!_ctx.getCamera().begin(EXPLORE_MODE)) {
		_ctx.writeSystemLog("Camera EXPLORE MODE init failed");
		return false;
	} else {
		_ctx.writeSystemLog("Camera EXPLORE MODE init succeeded");
	}

	if (!_ctx.getCamera().startStreaming(true)) {
		_ctx.writeSystemLog("Failed to start streaming");
		return false;
	} else {
		_ctx.writeSystemLog("Streaming started");
	}

	_isInitCamera = true;
	return true;
}

void ExploreState::endExploreMode() {
	_ctx.getCamera().end();
}
