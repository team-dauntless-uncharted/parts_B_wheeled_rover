#include "Controller/States/EscapeState.hpp"
#include "Controller/States/DetectionState.hpp"
#include "Controller/CansatController.hpp"

void EscapeState::onEnter() {
	_ctx.writeSystemLog("Entering EscapeState");
}

void EscapeState::onUpdate() {
	_ctx.getSerialWriter().log("Updating EscapeState");

	// _ctx.writeSystemLog("Change to DetectionState");
	// _ctx.changeState(std::make_unique<DetectionState>(_ctx));

	_ctx.getMotor().backward(150);
	delay(5000);
	_ctx.getMotor().stop();
}

void EscapeState::onExit() {
	_ctx.writeSystemLog("Exiting EscapeState");
}

State EscapeState::getState() const {
	return State::ESCAPE;
}