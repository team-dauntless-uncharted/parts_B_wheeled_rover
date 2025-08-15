#include "Controller/States/RecordingState.hpp"
#include "Controller/States/NavigationState.hpp"
#include "Controller/CansatController.hpp"

RecordingState* RecordingState::_instance = nullptr;

void RecordingState::onEnter() {
	_instance = this;
	_ctx.getSerialWriter().log("Entering RecordingState");
}

void RecordingState::onUpdate() {
	_ctx.getSerialWriter().log("Updating RecordingState");
	record(10000);
	_ctx.changeState(std::make_unique<NavigationState>(_ctx));
}

void RecordingState::onExit() {
	_instance = nullptr;
	_ctx.getSerialWriter().log("Exiting RecordingState");
	_ctx.getCamera().end();
}

State RecordingState::getState() const {
	return State::RECORDING;
}

void RecordingState::setRecordingMode() {
	_ctx.getCamera().end();

	delay(1000);
	
	if (!_ctx.getCamera().begin(VIDEO_MODE)) {
		_ctx.getSerialWriter().log("Camera init failed");
	}

	_ctx.getLogger().aviInit(CAM_IMGSIZE_QVGA_H, CAM_IMGSIZE_QVGA_V);
	if (!_ctx.getCamera().startStreaming(true, CamCB)) {
		_ctx.getSerialWriter().log("Failed to start streaming");
	}
	_ctx.getLogger().aviStart();
}

void RecordingState::record(int time_ms) {
	_ctx.getSerialWriter().log("Recording started");
	setRecordingMode();

	uint32_t start_time = millis();

	void* imgBuff = nullptr;
 	size_t imgSize = 0;

	while ((millis() - start_time) < time_ms) {
		delay(10);
	}

	_ctx.getLogger().aviEnd();
	_ctx.getCamera().startStreaming(false);
	_ctx.getSerialWriter().log("Recording finished");
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
		_ctx.getSerialWriter().log("Image captured");
	} else {
		_ctx.getSerialWriter().log("Failed to capture image");
	}
}