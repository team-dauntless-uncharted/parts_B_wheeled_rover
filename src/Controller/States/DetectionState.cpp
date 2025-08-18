#include "Controller/States/DetectionState.hpp"
#include "Controller/States/RecordingState.hpp"
#include "Controller/CansatController.hpp"

void DetectionState::onEnter() {
	_ctx.writeSystemLog("Entering DetectionState");
}

void DetectionState::onUpdate() {
	_ctx.getSerialWriter().log("Updating DetectionState");
}

void DetectionState::onExit() {
	_ctx.writeSystemLog("Exiting DetectionState");
}

State DetectionState::getState() const {
	return State::DETECTION;
}