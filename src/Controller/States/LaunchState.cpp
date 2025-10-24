/**
 * @file LaunchState.cpp
 */

#include "Controller/States/LaunchState.hpp"
#include "Controller/States/DropState.hpp"
#include "Controller/CansatController.hpp"

void LaunchState::onEnter() {
	_ctx.writeSystemLog("%lu: Entering LaunchState", millis());

	// 状態に対応したビープ音とLED点灯パターンを実行
	_ctx.getSpeaker().playState((int)State::LAUNCH);
	_ctx.setLed((int)State::LAUNCH);

	_startTime = millis();  // タイムアウト判定用の開始時刻を記録
}

void LaunchState::onUpdate() {
	_ctx.getSerialWriter().log("Updating LaunchState");

	// CdSセンサーで放出を検知（閾値未満=明るい=ペイロードベイから出た）
	// 放出を検知したら DROP モードに遷移する
    if (_ctx.getCds().read() < _ctx.getUserConfig().launchStateCdsThreshold) {
		_ctx.writeSystemLog("%lu: Detect separation. Change to DropState", millis());
		_ctx.changeState(std::make_unique<DropState>(_ctx));
		return;
    }

	// タイムアウト判定（config.jsonで設定可能）
	unsigned long elapsedTime = millis() - _startTime;
	_ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);
	if (elapsedTime > _ctx.getUserConfig().launchStateTimeoutThreshold) {
		_ctx.writeSystemLog("%lu: Timeout. Change to DropState", millis());
		_ctx.changeState(std::make_unique<DropState>(_ctx));
		return;
	}
}

void LaunchState::onExit() {
	_ctx.writeSystemLog("%lu: Exiting LaunchState", millis());

	// 次の状態（DROP）をSDカードに保存
	if (!_ctx.getSDLogger().writeState((int)State::DROP)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	// USE_FLAGが定義されている場合はFlashにも保存
	if (!_ctx.getFlashIO().writeState((int)State::DROP)) {
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

State LaunchState::getState() const {
	return State::LAUNCH;
}