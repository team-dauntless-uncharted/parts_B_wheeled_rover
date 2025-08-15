#pragma once
#include "Controller/ICansatState.hpp"

class LandingState : public ICansatState {
public:
	LandingState(CansatController& ctx) : _ctx(ctx) {}

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
	State getState() const override;

private:
	CansatController& _ctx;
};