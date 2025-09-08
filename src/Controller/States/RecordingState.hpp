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
	unsigned long _startTime;
	bool _isInitCamera = false;
	static RecordingState *_instance;
	bool _isRecordingOK = false;
	
	// エラーハンドリング用メンバ変数
	bool _recordingError = false;
	bool _aviRecordingActive = false;
	uint32_t _frameCount = 0;

	bool setRecordingMode();
	bool record(int time_ms);  // 戻り値をboolに変更
	void endRecordingMode();
	void handleCameraImage(CamImage img);
};