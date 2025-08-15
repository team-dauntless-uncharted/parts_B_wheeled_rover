#pragma once
#include "Controller/ICansatState.hpp"

class RecordingState : public ICansatState {
public:
	RecordingState(CansatController& ctx) : _ctx(ctx) {}

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
	State getState() const override;

private:
	CansatController& _ctx;

	void setRecordingMode();
	void record(int time_ms);
};