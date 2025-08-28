#include "Controller/States/NavigationState.hpp"
#include "Controller/CansatController.hpp"

void NavigationState::onEnter() {
    _ctx.writeSystemLog("Entering NavigationState");
}

void NavigationState::onUpdate() {
	_ctx.getSerialWriter().log("Updating NavigationState");

	// 直進のみ行う
	_ctx.getMotor().forward(100);
}

void NavigationState::onExit() {
    _ctx.writeSystemLog("Exiting NavigationState");
}

State NavigationState::getState() const {
	return State::NAVIGATION;
}