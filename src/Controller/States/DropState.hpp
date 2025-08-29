#pragma once
#include "Controller/ICansatState.hpp"

class DropState : public ICansatState {
public:
	DropState(CansatController& ctx) : _ctx(ctx) {}

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
	State getState() const override;

private:
	CansatController& _ctx;
	unsigned long _startTime;

	bool _landed = false;
	bool _impactDetected = false;
	unsigned long _impactTime = 0;
	unsigned long _stableStart = 0;
};