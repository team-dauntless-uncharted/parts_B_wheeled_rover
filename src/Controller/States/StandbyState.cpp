/**
 * @file StandbyState.cpp
 */

#include "Controller/States/StandbyState.hpp"
#include "Controller/States/LaunchState.hpp"
#include "Controller/CansatController.hpp"

void StandbyState::onEnter() {
	_ctx.writeSystemLog("%lu: Entering StandbyState", millis());

	// 状態に対応したビープ音とLED点灯パターンを実行
	_ctx.getSpeaker().playState((int)State::STANDBY);
	_ctx.setLed((int)State::STANDBY);

	_startTime = millis();  // タイムアウト判定用の開始時刻を記録
}

void StandbyState::onUpdate() {
	_ctx.getSerialWriter().log("Updating StandbyState");

	// 高度閾値判定（config.jsonで設定可能）
	if (_ctx.getGnss().getAltitude() > _ctx.getUserConfig().standbyStateAltThreshold) {
		_ctx.writeSystemLog("%lu: Above an altitude. Change to LaunchState", millis());
		_ctx.changeState(std::make_unique<LaunchState>(_ctx));
		return;
	}

	// タイムアウト判定
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

	// 次の状態（LAUNCH）をSDカードに保存
	if (!_ctx.getSDLogger().writeState((int)State::LAUNCH)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	// USE_FLAGが定義されている場合はFlashにも保存
	if (!_ctx.getFlashIO().writeState((int)State::LAUNCH)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
	}
#endif // USE_FLASH

	// カメラが未初期化の場合、PHOTO_MODEで初期化
	if (!_ctx.isInitCamera()) {
		if (!_ctx.getCamera().begin(PHOTO_MODE)) {
			return;
		}

		if (!_ctx.getCamera().startStreaming(true)) {
			return;
		}
		_ctx.setInitCamera(true);
	}

	// 状態遷移前に1枚撮影してSDカードに保存
	void* imgBuff = nullptr;
    size_t imgSize = 0;
    if (_ctx.getCamera().takePicture(&imgBuff, &imgSize)) {
        _ctx.getSerialWriter().log("Save taken picture to SD card...");
        _ctx.getSDLogger().saveJPEGImage(imgBuff, imgSize);
    } else {
		_ctx.getSerialWriter().log("Failed to take picture");
    }

	// カメラを終了して次の状態に備える
	_ctx.getCamera().end();
	_ctx.setInitCamera(false);
}

State StandbyState::getState() const {
	return State::STANDBY;
}