#include "Controller/States/CalibrationState.hpp"
#include "Controller/States/StandbyState.hpp"
#include "Controller/CansatController.hpp"

void CalibrationState::onEnter() {
	_ctx.writeSystemLog("Entering CalibrationState");

	_ctx.getSpeaker().playState((int)State::CALIBRATION);
	_ctx.setLed((int)State::CALIBRATION);

	_startTime = millis();
}

// センサのキャリブレーション
void CalibrationState::onUpdate() {
	_ctx.getSerialWriter().log("Updating CalibrationState");

	while (true) {
		CalibrationStatus calib = _ctx.getBno055().getCalibrationStatus();

		if (isFullyCalibrated(calib)) {
			_ctx.writeSystemLog("Calibration finished. Change to StandbyState");
			break;
		}

		if (calib.mag < 3) {
			_ctx.getLed(0).off();
		} else {
			_ctx.getLed(0).on();
		}

		if (calib.accel < 3) {
			_ctx.getLed(1).off();
		} else {
			_ctx.getLed(1).on();
		}

		if (calib.gyro < 3) {
			_ctx.getLed(2).off();
		} else {
			_ctx.getLed(2).on();
		}

		if (calib.system < 3) {
			_ctx.getLed(3).off();
		} else {
			_ctx.getLed(3).on();
		}

		long elapsedTime = millis() - _startTime;
		_ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);
    	if (elapsedTime > _ctx.getUserConfig().calibrationStateTimeoutThreshold) {
			_ctx.writeSystemLog("Timeout. Change to StandbyState");
			break;
    	}
	}

	_ctx.changeState(std::make_unique<StandbyState>(_ctx));
}

void CalibrationState::onExit() {
	_ctx.writeSystemLog("Exiting CalibrationState");

	if (!_ctx.getSDLogger().writeState((int)State::STANDBY)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	if (!_ctx.getFlashIO().writeState((int)State::STANDBY)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
	}
#endif // USE_FLASH
}

State CalibrationState::getState() const {
	return State::CALIBRATION;
}

bool CalibrationState::isFullyCalibrated(CalibrationStatus calib) {
    return (calib.system >= 1 && calib.gyro >= 3 && calib.accel >= 3 && calib.mag >= 3);
}