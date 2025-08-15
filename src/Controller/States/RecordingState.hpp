#pragma once
#include "Controller/ICansatState.hpp"
#include <Camera.h>

class RecordingState : public ICansatState {
public:
	RecordingState(CansatController& ctx) : _ctx(ctx) {}

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
	State getState() const override;

	static void CamCB(CamImage img);

private:
	CansatController& _ctx;
	static RecordingState *_instance;

	void setRecordingMode();
	void record(int time_ms);
	void handleCameraImage(CamImage img);
};