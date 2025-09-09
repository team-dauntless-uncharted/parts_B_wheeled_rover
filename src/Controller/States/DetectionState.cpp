#include "Controller/States/DetectionState.hpp"
#include "Controller/States/RecordingState.hpp"
#include "Controller/CansatController.hpp"

#include "a_parts_blackrock_inferencing.h"
#include <edge-impulse-sdk/dsp/image/image.hpp>

uint8_t *DetectionState::_image_buffer_base = nullptr;
uint8_t *DetectionState::_current_image_buffer = nullptr;

void DetectionState::onEnter() {
	_ctx.writeSystemLog("%lu: Entering DetectionState", millis());

	// _ctx.getSpeaker().playState((int)State::DETECTION);
	_ctx.setLed((int)State::DETECTION);

	if (!setDetectionMode()) {
		_ctx.writeSystemLog("%lu: Failed to set detection mode", millis());
	} else {
		_ctx.writeSystemLog("%lu: Detection mode set", millis());
		if (!beginEdgeImpulse()) {
			_ctx.writeSystemLog("%lu: Failed to initialize Edge Impulse", millis());
		} else {
			_ctx.writeSystemLog("%lu: Succeed to initialize Edge Impulse", millis());
		}
	}
}

void DetectionState::onUpdate() {
	_ctx.getSerialWriter().log("Updating DetectionState");

	_failedCount++;
	if (_failedCount >= _ctx.getUserConfig().detectionMaxFailedCount) {
		_ctx.writeSystemLog("%lu: Failed too many times. Change to RecordingState", millis());
		_ctx.changeState(std::make_unique<RecordingState>(_ctx));
		return;
	}

	if (_isInitEdgeImpulse) {
		// 画像を撮影する
    	void* imgBuff = nullptr;
    	size_t imgSize = 0;
    	if (!_ctx.getCamera().takePicture(&imgBuff, &imgSize)) {
			_ctx.getSerialWriter().log("Failed to take picture");
		   return;
		}

		if (!convertYUV422ToRGB888(imgBuff, imgSize)) {
			_ctx.getSerialWriter().log("Failed to convert YUV422 to RGB888");
			return;
		}

		if (!resizeImage()) {
			_ctx.getSerialWriter().log("Failed to resize image");
			return;
		}

		// if (!_ctx.getSDLogger().savePPMImage(_current_image_buffer, OUTPUT_WIDTH * OUTPUT_HEIGHT * 3)) {
		// 	_ctx.getSerialWriter().log("Failed to save resized image");
		// 	return;
		// }

		if (!detectObjects()) {
			_ctx.getSerialWriter().log("Failed to detect objects");
			return;
		}

		if (_result.has_detection) {
			int x = _result.detected_objects[0].x;
			_ctx.writeSystemLog("A-parts detected %f x=%d y=%d", _result.detected_objects[0].value, _result.detected_objects[0].x, _result.detected_objects[0].y);
			
			if (!_ctx.getSDLogger().savePPMImage(_current_image_buffer, OUTPUT_WIDTH * OUTPUT_HEIGHT * 3)) {
				_ctx.getSerialWriter().log("Failed to save resized image");
			 	return;
			}

			switch (_succeedCount) {
			case 0:
				_ctx.getMotor().turnLeft(150);
				delay(_ctx.getUserConfig().detectionTurn180delay);
				_ctx.getMotor().stop();
				delay(100);
				_ctx.getMotor().snakeForwardSmooth(150, 4000, 1000);
				delay(100);
				break;
			case 1:
				_ctx.writeSystemLog("%lu: A-parts detected. Changing to RecordingState, millis()");
		 		_ctx.changeState(std::make_unique<RecordingState>(_ctx));
				return;
			default:
				_succeedCount = 0;
				break;
			}
			_succeedCount++;
		}
	}

	_ctx.getMotor().turnLeft(150);
	delay(200);
	_ctx.getMotor().stop();
}

void DetectionState::onExit() {
	_ctx.writeSystemLog("%lu: Exiting DetectionState", millis());
	if (_isInitEdgeImpulse) {
		endEdgeImpulse();
	}

	if (_isInitCamera) {
		endDetectionMode();
	}

	if (!_ctx.getSDLogger().writeState((int)State::RECORDING)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	if (!_ctx.getFlashIO().writeState((int)State::RECORDING)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
	}
#endif // USE_FLASH
}

State DetectionState::getState() const {
	return State::DETECTION;
}

bool DetectionState::setDetectionMode() {
	if (!_ctx.getCamera().begin(DETECTION_MODE)) {
		return false;
	}

	if (!_ctx.getCamera().startStreaming(true)) {
		return false;
	}

	_isInitCamera = true;
	return true;
}

void DetectionState::endDetectionMode() {
	_ctx.getCamera().startStreaming(false);
	_ctx.getCamera().end();
	_isInitCamera = false;
}

bool DetectionState::beginEdgeImpulse(void) {
	_image_buffer_base = (uint8_t*)ei_malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * 3 + 32);
	if (_image_buffer_base == nullptr) {
		return false;
	}
    _current_image_buffer = (uint8_t *)ALIGN_PTR((uintptr_t)_image_buffer_base, 32);

	_isInitEdgeImpulse = true;
	return true;
}

void DetectionState::endEdgeImpulse(void) {
	if (_image_buffer_base) {
		ei_free(_image_buffer_base);
	}
	_image_buffer_base = nullptr;
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