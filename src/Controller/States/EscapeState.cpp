#include "Controller/States/EscapeState.hpp"
#include "Controller/States/DetectionState.hpp"
#include "Controller/CansatController.hpp"

#include "Utils/GeoUtils/GeoUtils.hpp"

void EscapeState::onEnter() {
	_ctx.writeSystemLog("Entering EscapeState");

	_ctx.getSpeaker().playState((int)State::ESCAPE);
	_ctx.setLed((int)State::ESCAPE);

	_startLatitude = _ctx.getGnss().getLatitude();
	_startLongitude = _ctx.getGnss().getLongitude();

	_startTime = millis();
}

void EscapeState::onUpdate() {
	_ctx.getSerialWriter().log("Updating EscapeState");

	unsigned long elapsedTime = millis() - _startTime;
	_ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);
	if (elapsedTime > _ctx.getUserConfig().escapeStateTimeoutThreshold) {
		_ctx.writeSystemLog("Timeout. Change to DetectionState");
		_ctx.changeState(std::make_unique<DetectionState>(_ctx));
		return;
	}

	pulseForward(150, 5);
	rockingEscape(150, 3);
	phaseEscape(150, 3);

	double latitude = _ctx.getGnss().getLatitude();
	double longitude = _ctx.getGnss().getLongitude();

	double distance = GeoUtils::haversineDistance(_startLatitude, _startLongitude, latitude, longitude);

	if (distance >= _ctx.getUserConfig().escapeStateDistanceThreshold) {
		_ctx.writeSystemLog("Escaped. Change to DetectionState");
		_ctx.changeState(std::make_unique<DetectionState>(_ctx));
		return;
	}
}

void EscapeState::onExit() {
	_ctx.writeSystemLog("Exiting EscapeState");

	if (!_ctx.getSDLogger().writeState((int)State::DETECTION)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	if (!_ctx.getFlashIO().writeState((int)State::DETECTION)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
	}
#endif // USE_FLASH

	if (!_ctx.isInitCamera()) {
		if (!_ctx.getCamera().begin(PHOTO_MODE)) {
			return;
		}

		if (!_ctx.getCamera().startStreaming(true)) {
			return;
		}
		_ctx.setInitCamera(true);
	}

	void* imgBuff = nullptr;
    size_t imgSize = 0;
    if (_ctx.getCamera().takePicture(&imgBuff, &imgSize)) {
        _ctx.getSerialWriter().log("Save taken picture to SD card...");
        _ctx.getSDLogger().saveJPEGImage(imgBuff, imgSize);
    } else {
		_ctx.getSerialWriter().log("Failed to take picture");
    }

	_ctx.getCamera().end();
	_ctx.setInitCamera(false);
}

State EscapeState::getState() const {
	return State::ESCAPE;
}

void EscapeState::pulseForward(int pwm, int repeat) {
	for (int i = 0; i < repeat; ++i) {
		_ctx.getMotor().forward(pwm);
		delay(200);
		_ctx.getMotor().stop();
		delay(200);
	}
}

void EscapeState::rockingEscape(int pwm, int repeat) {
	for (int i = 0; i < repeat; ++i) {
		_ctx.getMotor().forward(pwm);
		delay(200);
		_ctx.getMotor().stop();
		delay(150);
		_ctx.getMotor().backward(pwm);
		delay(200);
		_ctx.getMotor().stop();
		delay(150);
	}
}

void EscapeState::phaseEscape(int pwm, int repeat) {
	for (int i = 0; i < repeat; ++i) {
		_ctx.getMotor().leftForward(pwm);
		delay(150);

		_ctx.getMotor().rightForward(pwm);
		delay(150);

		_ctx.getMotor().stop();
		delay(200);
	}
}