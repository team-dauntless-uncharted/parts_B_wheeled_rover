#pragma once
#include "Controller/ICansatState.hpp"

class DropState : public ICansatState {
public:
	DropState(CansatController& ctx) : _ctx(ctx) {}

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;

private:
	CansatController& _ctx;
};