#pragma once
#include "Controller/ICansatState.hpp"

class DetectionState : public ICansatState {
public:
	DetectionState(CansatController& ctx) : _ctx(ctx) {}

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
	State getState() const override;

private:
	CansatController& _ctx;
};