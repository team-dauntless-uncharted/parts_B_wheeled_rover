#pragma once
#include "Controller/ICansatState.hpp"

class ExploreState : public ICansatState {
public:
	ExploreState(CansatController& ctx) : _ctx(ctx) {}

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
	State getState() const override;

private:
	CansatController& _ctx;

	double _prevLatitude = 0.0;
	double _prevLongitude = 0.0;
	int _sameCount = 0;
	const int SAME_LIMIT = 5;
};