#include "Controller/States/EscapeState.hpp"
#include "Controller/States/DetectionState.hpp"
#include "Controller/CansatController.hpp"

#include "Utils/GeoUtils/GeoUtils.hpp"

void EscapeState::onEnter() {
	_ctx.writeSystemLog("Entering EscapeState");

	_startLatitude = _ctx.getGnss().getLatitude();
	_startLongitude = _ctx.getGnss().getLongitude();
}

void EscapeState::onUpdate() {
	_ctx.getSerialWriter().log("Updating EscapeState");

	pulseForward(150, 5);
	rockingEscape(150, 3);
	phaseEscape(150, 3);
	singleWheelBackwardEscape(150, 2);
	
	double latitude = _ctx.getGnss().getLatitude();
	double longitude = _ctx.getGnss().getLongitude();

	double distance = GeoUtils::haversineDistance(_startLatitude, _startLongitude, latitude, longitude);

	if (distance >= 0.5) {
		_ctx.writeSystemLog("Change to DetectionState");
		_ctx.changeState(std::make_unique<DetectionState>(_ctx));
		return;
	}
}

void EscapeState::onExit() {
	_ctx.writeSystemLog("Exiting EscapeState");
}

State EscapeState::getState() const {
	return State::ESCAPE;
}

void EscapeState::pulseForward(int pwm, int repeat) {
	// 前進と停止を交互に繰り返して、引っかかりから抜け出す動作
	for (int i = 0; i < repeat; ++i) {
		_ctx.appendSensorLog();
		_ctx.getMotor().forward(pwm);
		delay(200);
		_ctx.appendSensorLog();
		_ctx.getMotor().stop();
		delay(200);
	}
}

void EscapeState::rockingEscape(int pwm, int repeat) {
	// 前進と後退を交互に繰り返して、前後に揺さぶりながら脱出を試みる動作
	for (int i = 0; i < repeat; ++i) {
		_ctx.appendSensorLog();
		_ctx.getMotor().forward(pwm);
		delay(200);
		_ctx.getMotor().stop();
		delay(150);
		_ctx.appendSensorLog();
		_ctx.getMotor().backward(pwm);
		delay(200);
		_ctx.getMotor().stop();
		delay(150);
	}
}

void EscapeState::phaseEscape(int pwm, int repeat) {
	// 左右の片輪を交互に前進させて、姿勢をずらしながら脱出を試みる動作
	for (int i = 0; i < repeat; ++i) {
		_ctx.appendSensorLog();
		_ctx.getMotor().leftForward(pwm);
		delay(150);

		_ctx.appendSensorLog();
		_ctx.getMotor().rightForward(pwm);
		delay(150);

		_ctx.getMotor().stop();
		delay(200);
	}
}

void EscapeState::singleWheelBackwardEscape(int pwm, int repeat) {
	// 片輪だけを後退させて車体をねじり、障害物から抜け出す動作
	for (int i = 0; i < repeat; ++i) {
		_ctx.appendSensorLog();
		_ctx.getMotor().leftBackward(pwm);
		delay(200);

		_ctx.getMotor().stop();
		delay(150);

		_ctx.appendSensorLog();
		_ctx.getMotor().rightBackward(pwm);
		delay(200);

		_ctx.getMotor().stop();
		delay(150);
	}
}
