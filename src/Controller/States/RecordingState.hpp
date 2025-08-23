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
	bool _isInitCamera = false;
	static RecordingState *_instance;

	bool setRecordingMode();
	void record(int time_ms);
	void endRecordingMode();
	void handleCameraImage(CamImage img);
};