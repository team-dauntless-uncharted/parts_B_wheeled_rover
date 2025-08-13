#pragma once
#include "Controller/ICansatState.hpp"

class StandbyState : public ICansatState {
public:
	StandbyState(CansatController& ctx) : _ctx(ctx) {}

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;

private:
	CansatController& _ctx;
};