/**
 * @file DetectionState.cpp
 */

#include "Controller/States/DetectionState.hpp"
#include "Controller/States/RecordingState.hpp"
#include "Controller/CansatController.hpp"

#include "a_parts_blackrock_inferencing.h"
#include <edge-impulse-sdk/dsp/image/image.hpp>

// Edge Impulse用の画像バッファ（静的メンバ変数）
uint8_t *DetectionState::_image_buffer_base = nullptr;
uint8_t *DetectionState::_current_image_buffer = nullptr;

void DetectionState::onEnter() {
	_ctx.writeSystemLog("%lu: Entering DetectionState", millis());

	// _ctx.getSpeaker().playState((int)State::DETECTION);
	_ctx.setLed((int)State::DETECTION);

	// DETECTION_MODE（QQVGA YUV422）でカメラを初期化
	if (!setDetectionMode()) {
		_ctx.writeSystemLog("%lu: Failed to set detection mode", millis());
	} else {
		_ctx.writeSystemLog("%lu: Detection mode set", millis());
		// Edge Impulse用の画像バッファを確保
		if (!beginEdgeImpulse()) {
			_ctx.writeSystemLog("%lu: Failed to initialize Edge Impulse", millis());
		} else {
			_ctx.writeSystemLog("%lu: Succeed to initialize Edge Impulse", millis());
		}
	}
}

void DetectionState::onUpdate() {
	_ctx.getSerialWriter().log("Updating DetectionState");

	// 失敗回数をカウントし、上限に達したらRecordingStateへ遷移
	_failedCount++;
	if (_failedCount >= _ctx.getUserConfig().detectionMaxFailedCount) {
		_ctx.writeSystemLog("%lu: Failed too many times. Change to RecordingState", millis());
		_ctx.changeState(std::make_unique<RecordingState>(_ctx));
		return;
	}

	if (_isInitEdgeImpulse) {
		// 画像を撮影する（QQVGA YUV422形式）
    	void* imgBuff = nullptr;
    	size_t imgSize = 0;
    	if (!_ctx.getCamera().takePicture(&imgBuff, &imgSize)) {
			_ctx.getSerialWriter().log("Failed to take picture");
		   return;
		}

		// YUV422をRGB888に変換
		if (!convertYUV422ToRGB888(imgBuff, imgSize)) {
			_ctx.getSerialWriter().log("Failed to convert YUV422 to RGB888");
			return;
		}

		// 96x96にリサイズ
		if (!resizeImage()) {
			_ctx.getSerialWriter().log("Failed to resize image");
			return;
		}

		// デバッグ用：リサイズ後の画像をSDカードに保存（コメントアウト中）
		// if (!_ctx.getSDLogger().savePPMImage(_current_image_buffer, OUTPUT_WIDTH * OUTPUT_HEIGHT * 3)) {
		// 	_ctx.getSerialWriter().log("Failed to save resized image");
		// 	return;
		// }

		// Edge Impulseで物体検出を実行
		if (!detectObjects()) {
			_ctx.getSerialWriter().log("Failed to detect objects");
			return;
		}

		// A-partsを検出した場合の処理
		if (_result.has_detection) {
			int x = _result.detected_objects[0].x;
			_ctx.writeSystemLog("A-parts detected %f x=%d y=%d", _result.detected_objects[0].value, _result.detected_objects[0].x, _result.detected_objects[0].y);

			// 検出した画像をSDカードに保存
			if (!_ctx.getSDLogger().savePPMImage(_current_image_buffer, OUTPUT_WIDTH * OUTPUT_HEIGHT * 3)) {
				_ctx.getSerialWriter().log("Failed to save resized image");
			 	return;
			}

			// 検出成功回数に応じた動作
			switch (_succeedCount) {
			case 0:
				// 1回目：180度回転してA-partsに接近
				_ctx.getMotor().turnLeft(150);
				delay(_ctx.getUserConfig().detectionTurn180delay);
				_ctx.getMotor().stop();
				delay(100);
				_ctx.getMotor().snakeForwardSmooth(150, 4000, 1000);
				delay(100);
				break;
			case 1:
				// 2回目：RecordingStateへ遷移（A-partsの近くに到達）
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

	// 検出できなかった場合、左旋回して探索を続ける
	_ctx.getMotor().turnLeft(150);
	delay(200);
	_ctx.getMotor().stop();
}

void DetectionState::onExit() {
	_ctx.writeSystemLog("%lu: Exiting DetectionState", millis());

	// Edge Impulseの終了（画像バッファ解放）
	if (_isInitEdgeImpulse) {
		endEdgeImpulse();
	}

	// カメラの終了
	if (_isInitCamera) {
		endDetectionMode();
	}

	// 次の状態（RECORDING）をSDカードに保存
	if (!_ctx.getSDLogger().writeState((int)State::RECORDING)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	// USE_FLAGが定義されている場合はFlashにも保存
	if (!_ctx.getFlashIO().writeState((int)State::RECORDING)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
	}
#endif // USE_FLASH
}

State DetectionState::getState() const {
	return State::DETECTION;
}

// DETECTION_MODE（QQVGA YUV422）でカメラを初期化
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

// カメラの終了
void DetectionState::endDetectionMode() {
	_ctx.getCamera().startStreaming(false);
	_ctx.getCamera().end();
	_isInitCamera = false;
}

// Edge Impulseの初期化：RGB888画像用のバッファを確保（32バイトアラインメント）
bool DetectionState::beginEdgeImpulse(void) {
	_image_buffer_base = (uint8_t*)ei_malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * 3 + 32);
	if (_image_buffer_base == nullptr) {
		return false;
	}
    _current_image_buffer = (uint8_t *)ALIGN_PTR((uintptr_t)_image_buffer_base, 32);

	_isInitEdgeImpulse = true;
	return true;
}

// Edge Impulseの終了：画像バッファを解放
void DetectionState::endEdgeImpulse(void) {
	if (_image_buffer_base) {
		ei_free(_image_buffer_base);
	}
	_image_buffer_base = nullptr;
	_current_image_buffer = nullptr;
	_isInitEdgeImpulse = false;
}

// YUV422形式（カメラ出力）をRGB888形式（ML入力）に変換
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

// 160x120の画像を96x96にリサイズ（crop_and_interpolate）
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

// Edge Impulseで物体検出を実行（run_classifier）
bool DetectionState::detectObjects(void) {
	// 結果をリセット
	_result.has_detection = false;
	_result.detection_count = 0;
	memset(&_result.detected_objects, 0, sizeof(_result.detected_objects));

	// 画像データのシグナルを作成
	ei::signal_t signal;
	signal.total_length = OUTPUT_WIDTH * OUTPUT_HEIGHT;
	signal.get_data = &get_image_data;

	ei_impulse_result_t ei_result = { 0 };

	// Edge Impulseの推論を実行
	EI_IMPULSE_ERROR err = run_classifier(&signal, &ei_result, false);
	if (err != EI_IMPULSE_OK) {
		return false;
	}

	// 検出結果（bounding boxes）を_resultに格納
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

// Edge Impulseの推論コールバック：RGB888データをfloat配列に変換
int DetectionState::get_image_data(size_t offset, size_t length, float *out_ptr) {
    size_t pixel_ix = offset * 3;
	size_t pixels_left = length;
	size_t out_ptr_ix = 0;

	// RGB888（3バイト）を24bitのfloat値に変換
	while (pixels_left != 0) {
		out_ptr[out_ptr_ix] = (_current_image_buffer[pixel_ix] << 16) + (_current_image_buffer[pixel_ix + 1] << 8) + _current_image_buffer[pixel_ix + 2];

		out_ptr_ix++;
		pixel_ix += 3;
		pixels_left--;
	}

    return 0;
}
