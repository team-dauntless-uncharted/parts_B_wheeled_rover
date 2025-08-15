#include "Controller/States/LandingState.hpp"
#include "Controller/States/NavigationState.hpp"
#include "Controller/CansatController.hpp"

void LandingState::onEnter() {
	_ctx.writeSystemLog("Entering LandingState");
}

void LandingState::onUpdate() {
	_ctx.getSerialWriter().log("Updating LandingState");

    // パラシュートの切り離し
    delay(5000);

    // ニクロム線を加熱してテグスを切る
    _ctx.getHeater().heat(150, 10000);

    delay(5000);

	_ctx.writeSystemLog("Changing to NavigationState");
	_ctx.changeState(std::make_unique<NavigationState>(_ctx));
}

void LandingState::onExit() {
	_ctx.writeSystemLog("Exiting LandingState");
}

State LandingState::getState() const {
	return State::LANDING;
}