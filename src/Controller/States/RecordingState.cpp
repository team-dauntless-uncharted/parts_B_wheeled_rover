#include "Controller/States/RecordingState.hpp"
#include "Controller/States/ExploreState.hpp"
#include "Controller/CansatController.hpp"

RecordingState* RecordingState::_instance = nullptr;

void RecordingState::onEnter() {
	_instance = this;
	_ctx.writeSystemLog("%lu: Entering RecordingState", millis());

	// _ctx.getSpeaker().playState((int)State::RECORDING);
	_ctx.setLed((int)State::RECORDING);

  	if (!_ctx.isConnectTwelite()) {
    	_ctx.getTwelite().on();
    	_ctx.setIsConnectTwelite(true);
  	}

	delay(200);

	if (!setRecordingMode()) {
		_ctx.writeSystemLog("%lu: Failed to set recording mode", millis());
		_recordingError = true;
	} else {
		_ctx.writeSystemLog("%lu: Recording mode set", millis());
	}

	_startTime = millis();
}

void RecordingState::onUpdate() {
	_ctx.getSerialWriter().log("Updating RecordingState");

	// エラー状態の場合は早期終了
	if (_recordingError) {
		_ctx.writeSystemLog("%lu: RecordingState in error state, transitioning to ExploreState", millis());
		_ctx.changeState(std::make_unique<ExploreState>(_ctx));
		return;
	}

	twelite::Packet pkt = twelite::TwelitePacket::makePacket(
		twelite::B_PARTS,
		twelite::A_PARTS,
		twelite::ReadyForCapture,
		0,
		NULL
	);
	_ctx.getTwelite().sendPacket(pkt);

	if (_ctx.getTwelite().receivePacket(pkt)) {
		if (twelite::TwelitePacket::match(pkt, twelite::A_PARTS, twelite::B_PARTS, twelite::ReadyForCaptureAck)) {
			_ctx.writeSystemLog("%lu: ReadyForCaptureAck received. Recording start", millis());
			_isRecordingOK = true;
		}
	}

	unsigned long elapsedTime = millis() - _startTime;
	_ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);
	if (elapsedTime > _ctx.getUserConfig().recordingTimeoutThreshold) {
		_ctx.writeSystemLog("%lu: Timeout. Recording start", millis());
		_isRecordingOK = true;
	}

	if (_isRecordingOK) {
		if (record(_ctx.getUserConfig().recordingTime)) {
			_ctx.writeSystemLog("%lu: Finished recording successfully. Changing to ExploreState", millis());
		} else {
			_ctx.writeSystemLog("%lu: Recording failed but partial video may be saved. Changing to ExploreState", millis());
		}
		_ctx.changeState(std::make_unique<ExploreState>(_ctx));
		return;
	}
}

void RecordingState::onExit() {
	_ctx.writeSystemLog("%lu: Exiting RecordingState", millis());
	
	// 録画エラーの緊急停止処理
	if (_aviRecordingActive) {
		_ctx.writeSystemLog("%lu: Emergency AVI stop during state exit", millis());
		_ctx.getSDLogger().aviEmergencyStop();
		_aviRecordingActive = false;
	}
	
	if (_isInitCamera) {
		endRecordingMode();
	}

	if (!_ctx.getSDLogger().writeState((int)State::EXPLORE)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	if (!_ctx.getFlashIO().writeState((int)State::EXPLORE)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
	}
#endif // USE_FLASH

	_instance = nullptr;
}

State RecordingState::getState() const {
	return State::RECORDING;
}

bool RecordingState::setRecordingMode() {
	if (!_ctx.getCamera().begin(VIDEO_MODE)) {
		_ctx.writeSystemLog("%lu: Camera begin failed", millis());
		return false;
	}

	if (!_ctx.getCamera().startStreaming(true, CamCB)) {
		_ctx.writeSystemLog("%lu: Camera streaming start failed", millis());
		return false;
	}

	_isInitCamera = true;
	return true;
}

bool RecordingState::record(int time_ms) {
	_ctx.writeSystemLog("%lu: Recording started", millis());
	
	// AVI初期化
	if (!_ctx.getSDLogger().aviInit(CAM_IMGSIZE_QVGA_H, CAM_IMGSIZE_QVGA_V)) {
		_ctx.writeSystemLog("%lu: AVI initialization failed: %s", millis(), 
			_ctx.getSDLogger().aviGetErrorMessage());
		return false;
	}
	
	// AVI録画開始
	if (!_ctx.getSDLogger().aviStart()) {
		_ctx.writeSystemLog("%lu: AVI recording start failed: %s", millis(),
			_ctx.getSDLogger().aviGetErrorMessage());
		return false;
	}
	
	_aviRecordingActive = true;
	_frameCount = 0;
	_recordingError = false;

	uint32_t start_time = millis();
	uint32_t last_status_time = start_time;

	// 録画ループ
	while ((millis() - start_time) < time_ms) {
		// エラーチェック
		if (_ctx.getSDLogger().aviHasFailed()) {
			_ctx.writeSystemLog("%lu: AVI recording error detected: %s", millis(),
				_ctx.getSDLogger().aviGetErrorMessage());
			break;  // エラー発生時はループを抜ける
		}
		
		delay(10);
	}

	// 録画終了処理
	bool success = true;
	if (!_ctx.getSDLogger().aviEnd()) {
		_ctx.writeSystemLog("%lu: AVI recording end failed: %s", millis(),
			_ctx.getSDLogger().aviGetErrorMessage());
		success = false;
	}
	
	_aviRecordingActive = false;
	
	// カメラストリーミング停止
	if (!_ctx.getCamera().startStreaming(false)) {
		_ctx.writeSystemLog("%lu: Camera streaming stop failed", millis());
		success = false;
	}
	
	// 録画結果の報告
	uint32_t actual_duration = millis() - start_time;
	if (success) {
		_ctx.writeSystemLog("%lu: Recording finished successfully. Duration: %u ms, Frames: %u", 
			millis(), actual_duration, _frameCount);
	} else {
		_ctx.writeSystemLog("%lu: Recording finished with errors. Duration: %u ms, Frames: %u", 
			millis(), actual_duration, _frameCount);
	}
	
	return success;
}

void RecordingState::endRecordingMode() {
	_ctx.getCamera().end();
	_isInitCamera = false;
}

void RecordingState::CamCB(CamImage img) {
	if (_instance) {
		_instance->handleCameraImage(img);
	}
}

void RecordingState::handleCameraImage(CamImage img) {
	if (!img.isAvailable()) {
		_ctx.writeSystemLog("%lu: Invalid camera image received", millis());
		return;
	}
	
	// 録画がアクティブでない場合は処理しない
	if (!_aviRecordingActive || _recordingError) {
		return;
	}

	void* imgBuff = img.getImgBuff();
	size_t imgSize = img.getImgSize();

	// フレーム追加
	if (!_ctx.getSDLogger().aviRecord(imgBuff, imgSize)) {
		_ctx.writeSystemLog("%lu: Frame recording failed: %s", millis(),
			_ctx.getSDLogger().aviGetErrorMessage());
		_recordingError = true;
		return;
	}
	
	_frameCount++;
}