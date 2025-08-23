#include "Controller/States/RecordingState.hpp"
#include "Controller/States/ExploreState.hpp"
#include "Controller/CansatController.hpp"

RecordingState* RecordingState::_instance = nullptr;

void RecordingState::onEnter() {
	_instance = this;
	_ctx.writeSystemLog("Entering RecordingState");

	if (!_ctx.isInitTwelite()) {
		_ctx.getTwelite().begin(Serial2, 115200);
		_ctx.setInitTwelite(true);
	}

	if (!setRecordingMode()) {
		_ctx.writeSystemLog("Failed to set recording mode");
	} else {
		_ctx.writeSystemLog("Recording mode set");
	}
}

void RecordingState::onUpdate() {
	_ctx.getSerialWriter().log("Updating RecordingState");

	twelite::Packet pkt = twelite::TwelitePacket::makePacket(
		twelite::B_PARTS,
		twelite::A_PARTS,
		twelite::ReadyForCapture,
		0,
		NULL
	);
	_ctx.getTwelite().sendPacket(pkt);

	unsigned long timeout_ms = millis();

	while (true) {
		if (_ctx.getTwelite().receivePacket(pkt)) {
			if (twelite::TwelitePacket::match(pkt, twelite::A_PARTS, twelite::B_PARTS, twelite::ReadyForCaptureAck)) {
				_ctx.writeSystemLog("ReadyForCaptureAck received");
				break;
			}
		}

		if ((millis() - timeout_ms) > 20000) {
			_ctx.writeSystemLog("A Parts timeout");
			break;
		}
	}

	record(10000);
	_ctx.writeSystemLog("Changing to ExploreState");
	_ctx.changeState(std::make_unique<ExploreState>(_ctx));
}

void RecordingState::onExit() {
	_ctx.writeSystemLog("Exiting RecordingState");
	if (_isInitCamera) {
		endRecordingMode();
	}

	if (!_ctx.getSDLogger().writeState(State::EXPLORE)) {
		_ctx.writeSystemLog("Failed to write state");
		// Flash
	}
	_instance = nullptr;
}

State RecordingState::getState() const {
	return State::RECORDING;
}

bool RecordingState::setRecordingMode() {
	if (!_ctx.getCamera().begin(VIDEO_MODE)) {
		_ctx.writeSystemLog("Camera VIDEO MODE init failed");
		return false;
	} else {
		_ctx.writeSystemLog("Camera VIDEO MODE init succeeded");
	}

	if (!_ctx.getCamera().startStreaming(true, CamCB)) {
		_ctx.writeSystemLog("Failed to start streaming");
		return false;
	} else {
		_ctx.writeSystemLog("Streaming started");
	}

	_isInitCamera = true;
	return true;
}

void RecordingState::record(int time_ms) {
	_ctx.writeSystemLog("Recording started");
	
	// 修正: aviInitの戻り値をチェック
	if (!_ctx.getSDLogger().aviInit(CAM_IMGSIZE_QVGA_H, CAM_IMGSIZE_QVGA_V)) {
		_ctx.writeSystemLog("AVI initialization failed");
		return;
	}
	
	_ctx.getSDLogger().aviStart();

	uint32_t start_time = millis();

	void* imgBuff = nullptr;
 	size_t imgSize = 0;

	while ((millis() - start_time) < time_ms) {
		delay(10);
	}

	_ctx.getSDLogger().aviEnd();
	_ctx.getCamera().startStreaming(false);
	_ctx.writeSystemLog("Recording finished");
}

void RecordingState::endRecordingMode() {
	_ctx.getCamera().end();
}

void RecordingState::CamCB(CamImage img) {
	if (_instance) {
		_instance->handleCameraImage(img);
	}
}

void RecordingState::handleCameraImage(CamImage img) {
	if (img.isAvailable()) {
		void* imgBuff = img.getImgBuff();
		size_t imgSize = img.getImgSize();

		_ctx.getSDLogger().aviRecord(imgBuff, imgSize);
	} else {
		_ctx.writeSystemLog("Camera image is not available");
	}
}