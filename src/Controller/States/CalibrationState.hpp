#pragma once
#include "Controller/ICansatState.hpp"

#include <BNO055Library.h>

class CalibrationState : public ICansatState {
public:
	CalibrationState(CansatController& ctx) : _ctx(ctx) {}

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
	State getState() const override;

private:
	CansatController& _ctx;
	unsigned long _startTime;

	bool isFullyCalibrated(CalibrationStatus calib);
};