/**
 * @file ExploreState.cpp
 */

#include "Controller/States/ExploreState.hpp"
#include "Controller/CansatController.hpp"

void ExploreState::onEnter() {
	_ctx.writeSystemLog("%lu: Entering ExploreState", millis());

	// _ctx.getSpeaker().playState((int)State::EXPLORE);
	_ctx.setLed((int)State::EXPLORE);

	// PHOTO_MODEでカメラを初期化
	if (!setPhotoMode()) {
		_ctx.writeSystemLog("%lu: Failed to set explore mode", millis());
	} else {
		_ctx.writeSystemLog("%lu: Explore mode set", millis());
	}
}

void ExploreState::onUpdate() {
	_ctx.getSerialWriter().log("Updating ExploreState");

	// 蛇行前進（PWM150、2秒間、周期1ms）
	// // 適当に移動
	_ctx.getMotor().snakeForwardSmooth(150, 2000, 1);
	// _ctx.getMotor().forward(150);
	// delay(random(500, 1500));
	// _ctx.getMotor().stop();
	delay(100);

	// ランダムに左右旋回（200-500ms）
	if (random(2) == 0) {
		_ctx.getMotor().turnLeft(150);
	} else {
		_ctx.getMotor().turnRight(150);
	}
	delay(random(200, 500));
	_ctx.getMotor().stop();
	delay(100);

	// 画像を撮影してSDカードに保存
    void* imgBuff = nullptr;
    size_t imgSize = 0;
    if (_ctx.getCamera().takePicture(&imgBuff, &imgSize)) {
        _ctx.getSerialWriter().log("Save taken picture to SD card...");
        _ctx.getSDLogger().saveJPEGImage(imgBuff, imgSize);
    } else {
		_ctx.getSerialWriter().log("Failed to take picture");
    }
}

void ExploreState::onExit() {
	_ctx.writeSystemLog("%lu: Exiting ExploreState", millis());

	// カメラの終了
	if (_isInitCamera) {
		endPhotoMode();
	}
}

State ExploreState::getState() const {
	return State::EXPLORE;
}

// PHOTO_MODEでカメラを初期化
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

// カメラの終了
void ExploreState::endPhotoMode() {
	_ctx.getCamera().end();
}
