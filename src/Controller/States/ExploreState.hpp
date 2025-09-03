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
	bool _isInitCamera = false;

	bool setPhotoMode();
	void endPhotoMode();
};