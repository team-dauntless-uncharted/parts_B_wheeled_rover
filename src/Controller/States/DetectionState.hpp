#pragma once

#include <Arduino.h>
#include "Controller/ICansatState.hpp"

#define EI_CAMERA_RAW_FRAME_BUFFER_COLS 160
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS 120
#define EI_CAMERA_RAW_FRAME_BUFFER_BYTES 2

#define OUTPUT_WIDTH 96
#define OUTPUT_HEIGHT 96

#define ALIGN_PTR(p,a)   ((p & (a-1)) ?(((uintptr_t)p + a) & ~(uintptr_t)(a-1)) : p)

struct DetectionObject {
	float value;
	uint32_t x;
	uint32_t y;
};

struct DetectionResult {
	bool has_detection;
	uint32_t detection_count;
	DetectionObject detected_objects[10];
};

class DetectionState : public ICansatState {
public:
	DetectionState(CansatController& ctx) : _ctx(ctx), _result() {}

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
	State getState() const override;

private:
	CansatController& _ctx;
	int _failedCount = 0;
	bool _isInitCamera = false;
	bool _isInitEdgeImpulse = false;
	DetectionResult _result;

	bool setDetectionMode();
	void endDetectionMode();

	static uint8_t *_image_buffer_base;
	static uint8_t *_current_image_buffer;
	static int get_image_data(size_t offset, size_t length, float *out_ptr);

	bool beginEdgeImpulse(void);
	void endEdgeImpulse(void);
	
	bool convertYUV422ToRGB888(const uint8_t *yuv_buffer, size_t yuv_size);
	bool resizeImage(void);
	bool detectObjects(void);
};