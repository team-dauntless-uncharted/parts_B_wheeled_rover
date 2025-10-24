/**
 * @file EscapeState.cpp
 */

#include "Controller/States/EscapeState.hpp"
#include "Controller/States/DetectionState.hpp"
#include "Controller/CansatController.hpp"

void EscapeState::onEnter() {
	_ctx.writeSystemLog("%lu: Entering EscapeState", millis());

	// _ctx.getSpeaker().playState((int)State::ESCAPE);
	_ctx.setLed((int)State::ESCAPE);

	_startTime = millis();  // タイムアウト判定用の開始時刻を記録
	_count = 0;             // 脱出パターンのカウンタを初期化
}

void EscapeState::onUpdate() {
    _ctx.getSerialWriter().log("Updating EscapeState");
    unsigned long elapsedTime = millis() - _startTime;
    _ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);

    // タイムアウト判定（config.jsonで設定可能）
    if (elapsedTime > _ctx.getUserConfig().escapeStateTimeoutThreshold) {
        _ctx.writeSystemLog("%lu: Timeout. Change to DetectionState", millis());
        _ctx.changeState(std::make_unique<DetectionState>(_ctx));
        return;
    }

    // 4つのダイナミックなパターンをローテーション
    switch (_count % 4) {
        case 0:
			pulseForward(100, 3);            // パルス前進
            break;
        case 1:
            rockingEscape(100, 2);           // 前後左右ロッキング
            break;
        case 2:
            phaseEscape(100, 2);             // S字カーブ
            break;
        case 3:
            spiralEscape(100, 2);            // 渦巻き動作
            break;
    }

    _count++;

	// 4パターン実行後、加速度センサーで姿勢を確認してDetectionStateへ遷移
	if (_count >= 4) {
        delay(1000);

        // Z軸加速度が負（逆さま）の場合、前進して姿勢を正す
        if (_ctx.getBno055().getAcceleration().z < 0) {
            _ctx.getMotor().forward(150);
            delay(1500);
        }
        _ctx.getMotor().stop();

        _ctx.writeSystemLog("%lu: Timeout. Change to DetectionState", millis());
        _ctx.changeState(std::make_unique<DetectionState>(_ctx));
        return;
	}
}

void EscapeState::onExit() {
	_ctx.writeSystemLog("%lu: Exiting EscapeState", millis());

	// 次の状態（DETECTION）をSDカードに保存
	if (!_ctx.getSDLogger().writeState((int)State::DETECTION)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	// USE_FLAGが定義されている場合はFlashにも保存
	if (!_ctx.getFlashIO().writeState((int)State::DETECTION)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
	}
#endif // USE_FLASH

	// カメラが未初期化の場合、PHOTO_MODEで初期化
	if (!_ctx.isInitCamera()) {
		if (!_ctx.getCamera().begin(PHOTO_MODE)) {
			return;
		}

		if (!_ctx.getCamera().startStreaming(true)) {
			return;
		}
		_ctx.setInitCamera(true);
	}

	// 状態遷移前に1枚撮影してSDカードに保存
	void* imgBuff = nullptr;
    size_t imgSize = 0;
    if (_ctx.getCamera().takePicture(&imgBuff, &imgSize)) {
        _ctx.getSerialWriter().log("Save taken picture to SD card...");
        _ctx.getSDLogger().saveJPEGImage(imgBuff, imgSize);
    } else {
		_ctx.getSerialWriter().log("Failed to take picture");
    }

	// カメラを終了して次の状態に備える
	_ctx.getCamera().end();
	_ctx.setInitCamera(false);
}

State EscapeState::getState() const {
	return State::ESCAPE;
}

// パルス前進：断続的に前進と停止を繰り返す
void EscapeState::pulseForward(int pwm, int repeat) {
	for (int i = 0; i < repeat; ++i) {
		_ctx.getMotor().forward(pwm);
		delay(200);
		_ctx.getMotor().stop();
		delay(200);
	}
}

// ロッキング脱出：前後左右のロッキング動作でコンテナから脱出を試みる
void EscapeState::rockingEscape(int pwm, int repeat) {
    for (int i = 0; i < repeat; ++i) {
        // 前進
        _ctx.getMotor().forward(pwm);
        delay(250);
        _ctx.getMotor().stop();
        delay(100);

        // 右に旋回
        _ctx.getMotor().turnRight(pwm);
        delay(180);
        _ctx.getMotor().stop();
        delay(100);

        // 後退
        _ctx.getMotor().backward(pwm);
        delay(250);
        _ctx.getMotor().stop();
        delay(100);

        // 左に旋回（元の向きに戻す）
        _ctx.getMotor().turnLeft(pwm);
        delay(180);
        _ctx.getMotor().stop();
        delay(100);
    }
}

// 位相脱出：S字カーブのような動作で位置を変えながら最終的に戻る
void EscapeState::phaseEscape(int pwm, int repeat) {
    for (int i = 0; i < repeat; ++i) {
        // 右斜め前に移動
        _ctx.getMotor().rightForward(pwm);
        delay(200);
        _ctx.getMotor().leftForward(pwm * 0.7);  // 左を少し弱く
        delay(200);
        _ctx.getMotor().stop();
        delay(100);

        // 左斜め前に移動
        _ctx.getMotor().leftForward(pwm);
        delay(200);
        _ctx.getMotor().rightForward(pwm * 0.7);  // 右を少し弱く
        delay(200);
        _ctx.getMotor().stop();
        delay(100);

        // 右斜め後ろに移動（戻る）
        _ctx.getMotor().rightBackward(pwm);
        delay(200);
        _ctx.getMotor().leftBackward(pwm * 0.7);
        delay(200);
        _ctx.getMotor().stop();
        delay(100);

        // 左斜め後ろに移動（戻る）
        _ctx.getMotor().leftBackward(pwm);
        delay(200);
        _ctx.getMotor().rightBackward(pwm * 0.7);
        delay(200);
        _ctx.getMotor().stop();
        delay(100);
    }
}

// 渦巻き脱出：渦巻き状の動作で周囲を探索してから中心に戻る
void EscapeState::spiralEscape(int pwm, int repeat) {
    for (int i = 0; i < repeat; ++i) {
        int duration = 150 + (i * 50);  // 徐々に動作時間を延ばす

        // 前進しながら右旋回
        _ctx.getMotor().rightForward(pwm);
        delay(duration);
        _ctx.getMotor().leftForward(pwm * 0.5);
        delay(duration);
        _ctx.getMotor().stop();
        delay(100);

        // 90度回転
        _ctx.getMotor().turnLeft(pwm);
        delay(150);
        _ctx.getMotor().stop();
        delay(100);
    }

    _ctx.getMotor().stop();
}
