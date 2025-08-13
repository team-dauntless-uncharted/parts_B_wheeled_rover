#include "StandbyState.hpp"
#include "Controller/CansatController.hpp"

void StandbyState::onEnter() {
	_ctx.getSerialWriter().log("Entering StandbyState");
}

// TODO: センサのキャリブレーション
void StandbyState::onUpdate() {
	_ctx.getSerialWriter().log("Updating StandbyState");
}

void StandbyState::onExit() {
	_ctx.getSerialWriter().log("Exiting StandbyState");
}