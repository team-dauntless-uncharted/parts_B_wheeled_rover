#include "Controller/States/RecordingState.hpp"
#include "Controller/States/ExploreState.hpp"
#include "Controller/CansatController.hpp"

RecordingState* RecordingState::_instance = nullptr;

void RecordingState::onEnter() {
	_instance = this;
	_ctx.writeSystemLog("Entering RecordingState");
}

void RecordingState::onUpdate() {
	_ctx.getSerialWriter().log("Updating RecordingState");

	Packet pkt = twelite::TwelitePacket::makePacket(
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
	_instance = nullptr;
	_ctx.writeSystemLog("Exiting RecordingState");
	_ctx.getCamera().end();
}

State RecordingState::getState() const {
	return State::RECORDING;
}

void RecordingState::setRecordingMode() {
	_ctx.getCamera().end();

	delay(1000);
	
	if (!_ctx.getCamera().begin(VIDEO_MODE)) {
		_ctx.writeSystemLog("Camera VIDEO MODE init failed");
	} else {
		_ctx.writeSystemLog("Camera VIDEO MODE init succeeded");
	}

	_ctx.getLogger().aviInit(CAM_IMGSIZE_QVGA_H, CAM_IMGSIZE_QVGA_V);
	if (!_ctx.getCamera().startStreaming(true, CamCB)) {
		_ctx.writeSystemLog("Failed to start streaming");
	} else {
		_ctx.writeSystemLog("Streaming started");
	}
	_ctx.getLogger().aviStart();
}

void RecordingState::record(int time_ms) {
	_ctx.writeSystemLog("Recording started");
	setRecordingMode();

	uint32_t start_time = millis();

	void* imgBuff = nullptr;
 	size_t imgSize = 0;

	while ((millis() - start_time) < time_ms) {
		delay(10);
	}

	_ctx.getLogger().aviEnd();
	_ctx.getCamera().startStreaming(false);
	_ctx.writeSystemLog("Recording finished");
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

		_ctx.getLogger().aviRecord(imgBuff, imgSize);
	} else {
		_ctx.writeSystemLog("Camera image is not available");
	}
}