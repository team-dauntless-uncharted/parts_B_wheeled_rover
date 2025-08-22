#pragma once
#include "Controller/ICansatState.hpp"

class EscapeState : public ICansatState {
public:
	EscapeState(CansatController& ctx) : _ctx(ctx) {}

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
	State getState() const override;

private:
	CansatController& _ctx;

	double _startLatitude;
	double _startLongitude;

	// 脱出アルゴリズム
	void pulseForward(int pwm, int repeat);
	void rockingEscape(int pwm, int repeat);
	void phaseEscape(int pwm, int repeat);
	void singleWheelBackwardEscape(int pwm, int repeat);
};