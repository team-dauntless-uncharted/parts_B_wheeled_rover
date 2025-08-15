#include "RecordingState.hpp"
#include "NavigationState.hpp"
#include "Controller/CansatController.hpp"

void RecordingState::onEnter() {
	_ctx.getSerialWriter().log("Entering RecordingState");

	setRecordingMode();
}

void RecordingState::onUpdate() {
	_ctx.getSerialWriter().log("Updating RecordingState");

	record(10000);

	_ctx.changeState(std::make_unique<NavigationState>(_ctx));
}

void RecordingState::onExit() {
	_ctx.getSerialWriter().log("Exiting RecordingState");

	_ctx.getCamera().startStreaming(false);
	_ctx.getCamera().end();
}

State RecordingState::getState() const {
	return State::RECORDING;
}

void RecordingState::setRecordingMode() {
	_ctx.getCamera().startStreaming(false);
	_ctx.getCamera().end();

	delay(1000);
	
	if (!_ctx.getCamera().begin(VIDEO_MODE)) {
		_ctx.getSerialWriter().log("Camera init failed");
	}

	_ctx.getLogger().aviInit(CAM_IMGSIZE_QVGA_H, CAM_IMGSIZE_QVGA_V);
	_ctx.getCamera().startStreaming(true);
}

void RecordingState::record(int time_ms) {
	_ctx.getSerialWriter().log("Recording started");
	uint32_t start_time = millis();

	void* imgBuff = nullptr;
 	size_t imgSize = 0;

	while ((millis() - start_time) < time_ms) {
   		if (_ctx.getCamera().takePicture(&imgBuff, &imgSize)) {
       		_ctx.getSerialWriter().log("Save taken picture to SD card...");
       		_ctx.getLogger().saveJPEGImage(imgBuff, imgSize);
   		} else {
			_ctx.getSerialWriter().log("Failed to take picture");
  		}
	}

	_ctx.getLogger().aviEnd();
	_ctx.getSerialWriter().log("Recording finished");
}