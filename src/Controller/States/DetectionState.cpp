#include "Controller/States/DetectionState.hpp"
#include "Controller/States/RecordingState.hpp"
#include "Controller/CansatController.hpp"

#include "spresense_fomo_inferencing.h"
#include <edge-impulse-sdk/dsp/image/image.hpp>

uint8_t *DetectionState::_current_image_buffer = nullptr;

void DetectionState::onEnter() {
	_ctx.writeSystemLog("Entering DetectionState");

	if (!setDetectionMode()) {
		_ctx.writeSystemLog("Failed to set detection mode");
	} else {
		_ctx.writeSystemLog("Detection mode set");
	}

	if (!beginEdgeImpulse()) {
		_ctx.writeSystemLog("Failed to initialize Edge Impulse");
	}
}

void DetectionState::onUpdate() {
	_ctx.getSerialWriter().log("Updating DetectionState");

	if (_isInitEdgeImpulse) {
		// 画像を撮影する
    	void* imgBuff = nullptr;
    	size_t imgSize = 0;
    	if (!_ctx.getCamera().takePicture(&imgBuff, &imgSize)) {
     	   _ctx.writeSystemLog("Failed to take picture");
		   return;
		}

		if (!convertYUV422ToRGB888(imgBuff, imgSize)) {
			_ctx.writeSystemLog("Failed to convert YUV422 to RGB888");
			return;
		}

		if (!resizeImage()) {
			_ctx.writeSystemLog("Failed to resize image");
			return;
		}

		// if (!_ctx.getLogger().savePPMImage(_current_image_buffer, OUTPUT_WIDTH * OUTPUT_HEIGHT * 3)) {
		// 	_ctx.writeSystemLog("Failed to save resized image");
		// 	return;
		// }

		if (!detectObjects()) {
			_ctx.writeSystemLog("Failed to detect objects");
			return;
		}

		if (_result.has_detection) {
			int x = _result.detected_objects[0].x;
			_ctx.getSerialWriter().logf("100kinsat detected %f x=%d y=%d", _result.detected_objects[0].value, _result.detected_objects[0].x, _result.detected_objects[0].y);

		    if (x >= 43 && x <= 52) {
				_ctx.writeSystemLog("Changing to RecordingState");
				_ctx.changeState(std::make_unique<RecordingState>(_ctx));
				return;
		    } else if (x >= 0 && x <= 42) {
				// 右に回転
				_ctx.getMotor().turnRight(150);
				delay(100);
				_ctx.getMotor().stop();
    		} else if (x >= 53 && x <= 95) {
				// 左に回転
				_ctx.getMotor().turnLeft(150);
				delay(100);
				_ctx.getMotor().stop();
			} else {
				// 適当に回転
			}
		} else {
			_ctx.getMotor().turnLeft(150);
			delay(300);
			_ctx.getMotor().stop();
		}
	}

	_failedCount++;
	if (_failedCount >= _ctx.getUserConfig().detectionMaxFailedCount) {
		_ctx.writeSystemLog("DetectionState: Failed too many times. Change to RecordingState");
		_ctx.changeState(std::make_unique<RecordingState>(_ctx));
		return;
	}
}

void DetectionState::onExit() {
	_ctx.writeSystemLog("Exiting DetectionState");
	if (_isInitEdgeImpulse) {
		endEdgeImpulse();
	}

	if (_isInitCamera) {
		endDetectionMode();
	}

	if (!_ctx.getSDLogger().writeState((int)State::RECORDING)) {
		_ctx.writeSystemLog("Failed to write state");
	}

#ifdef USE_FLASH
	if (!_ctx.getFlashIO().writeState((int)State::RECORDING)) {
		_ctx.writeSystemLog("Failed to write state");
	}
#endif // USE_FLASH
}

State DetectionState::getState() const {
	return State::DETECTION;
}

bool DetectionState::setDetectionMode() {
	if (!_ctx.getCamera().begin(DETECTION_MODE)) {
		_ctx.writeSystemLog("Camera DETECTION MODE init failed");
		return false;
	} else {
		_ctx.writeSystemLog("Camera DETECTION MODE init succeeded");
	}

	if (!_ctx.getCamera().startStreaming(true)) {
		_ctx.writeSystemLog("Failed to start streaming");
		return false;
	} else {
		_ctx.writeSystemLog("Streaming started");
	}

	_isInitCamera = true;
	return true;
}

void DetectionState::endDetectionMode() {
	_ctx.getCamera().end();
}

bool DetectionState::beginEdgeImpulse(void) {
	_current_image_buffer = (uint8_t*)ei_malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * 3 + 32);
    _current_image_buffer = (uint8_t *)ALIGN_PTR((uintptr_t)_current_image_buffer, 32);

	if (_current_image_buffer == nullptr) {
		return false;
	}

	_isInitEdgeImpulse = true;
	return true;
}

void DetectionState::endEdgeImpulse(void) {
	ei_free(_current_image_buffer);
	_current_image_buffer = nullptr;
	_isInitEdgeImpulse = false;
}

bool DetectionState::convertYUV422ToRGB888(const uint8_t *yuv_buffer, size_t yuv_size) {
	if (!yuv_buffer) {
		return false;
	}

	if (ei::EIDSP_OK != ei::image::processing::yuv422_to_rgb888(
			_current_image_buffer, yuv_buffer, yuv_size, ei::image::processing::BIG_ENDIAN_ORDER)) {
		return false;
	}

	return true;
}

bool DetectionState::resizeImage(void) {
	ei::image::processing::crop_and_interpolate_rgb888(
		_current_image_buffer,
		EI_CAMERA_RAW_FRAME_BUFFER_COLS,
		EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
		_current_image_buffer,
		OUTPUT_WIDTH,
		OUTPUT_HEIGHT
	);

	return true;
}

bool DetectionState::detectObjects(void) {
	_result.has_detection = false;
	_result.detection_count = 0;
	memset(&_result.detected_objects, 0, sizeof(_result.detected_objects));

	ei::signal_t signal;
	signal.total_length = OUTPUT_WIDTH * OUTPUT_HEIGHT;
	signal.get_data = &get_image_data;

	ei_impulse_result_t ei_result = { 0 };

	EI_IMPULSE_ERROR err = run_classifier(&signal, &ei_result, false);
	if (err != EI_IMPULSE_OK) {
		_ctx.writeSystemLog("run_classifier failed");
		return false;
	}

	for (uint32_t i = 0; i < ei_result.bounding_boxes_count && i < 10; i++) {
		ei_impulse_result_bounding_box_t bb = ei_result.bounding_boxes[i];
		if (bb.value > 0) {
			_result.detected_objects[_result.detection_count].value = bb.value;
			_result.detected_objects[_result.detection_count].x = bb.x;
			_result.detected_objects[_result.detection_count].y = bb.y;

			_result.has_detection = true;
			_result.detection_count++;
		}
	}

	return true;
}

int DetectionState::get_image_data(size_t offset, size_t length, float *out_ptr) {
    size_t pixel_ix = offset * 3;
	size_t pixels_left = length;
	size_t out_ptr_ix = 0;
    
	while (pixels_left != 0) {
		out_ptr[out_ptr_ix] = (_current_image_buffer[pixel_ix] << 16) + (_current_image_buffer[pixel_ix + 1] << 8) + _current_image_buffer[pixel_ix + 2];

		out_ptr_ix++;
		pixel_ix += 3;
		pixels_left--;
	}

    return 0;
}