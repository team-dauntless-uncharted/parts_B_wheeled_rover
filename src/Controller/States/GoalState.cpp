#include "Controller/States/GoalState.hpp"
#include "Controller/CansatController.hpp"

void GoalState::onEnter() {
	_ctx.writeSystemLog("Entering GoalState");
}

void GoalState::onUpdate() {
	_ctx.getSerialWriter().log("Updating GoalState");

	// LED点滅
	_ctx.getLed(2).on();
	delay(1000);
	_ctx.getLed(2).off();
	delay(1000);
}

void GoalState::onExit() {
	_ctx.writeSystemLog("Exiting GoalState");
}

State GoalState::getState() const {
	return State::GOAL;
}