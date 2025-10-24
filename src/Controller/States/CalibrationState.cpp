/**
 * @file CalibrationState.cpp
 */

#include "Controller/States/CalibrationState.hpp"
#include "Controller/States/StandbyState.hpp"
#include "Controller/CansatController.hpp"

void CalibrationState::onEnter() {
	_ctx.writeSystemLog("%lu: Entering CalibrationState", millis());

	// 状態に対応したビープ音とLED点灯パターンを実行
	_ctx.getSpeaker().playState((int)State::CALIBRATION);
	_ctx.setLed((int)State::CALIBRATION);

	_startTime = millis();  // タイムアウト判定用の開始時刻を記録
}

// センサのキャリブレーション
// 本来の実装では以下の処理を行う予定：
// 1. BNO055の各センサー（mag, accel, gyro, system）のキャリブレーション状態を取得
// 2. 各センサーのキャリブレーション進捗をLED0-LED3で表示
// 3. 全センサーのキャリブレーションが完了するか、タイムアウト（config.jsonで設定）まで待機
// 4. 完了またはタイムアウトでStandbyStateへ遷移
void CalibrationState::onUpdate() {
	_ctx.getSerialWriter().log("Updating CalibrationState");

	// キャリブレーション処理（現在コメントアウト中）
	// while (true) {
	// 	CalibrationStatus calib = _ctx.getBno055().getCalibrationStatus();
	//
	// 	// 全センサーのキャリブレーション完了判定
	// 	if (isFullyCalibrated(calib)) {
	// 		_ctx.writeSystemLog("%lu: Calibration finished. Change to StandbyState", millis());
	// 		break;
	// 	}
	//
	// 	// 地磁気センサーのキャリブレーション進捗をLED0で表示
	// 	if (calib.mag < 3) {
	// 		_ctx.getLed(0).off();
	// 	} else {
	// 		_ctx.getLed(0).on();
	// 	}
	//
	// 	// 加速度センサーのキャリブレーション進捗をLED1で表示
	// 	if (calib.accel < 3) {
	// 		_ctx.getLed(1).off();
	// 	} else {
	// 		_ctx.getLed(1).on();
	// 	}
	//
	// 	// ジャイロセンサーのキャリブレーション進捗をLED2で表示
	// 	if (calib.gyro < 3) {
	// 		_ctx.getLed(2).off();
	// 	} else {
	// 		_ctx.getLed(2).on();
	// 	}
	//
	// 	// システム全体のキャリブレーション進捗をLED3で表示
	// 	if (calib.system < 3) {
	// 		_ctx.getLed(3).off();
	// 	} else {
	// 		_ctx.getLed(3).on();
	// 	}
	//
	// 	// タイムアウト判定（デフォルト5分）
	// 	long elapsedTime = millis() - _startTime;
	// 	_ctx.getSerialWriter().logf("Elapsed time: %lu", elapsedTime);
    // 	if (elapsedTime > _ctx.getUserConfig().calibrationStateTimeoutThreshold) {
	// 		_ctx.writeSystemLog("%lu: Timeout. Change to StandbyState", millis());
	// 		break;
    // 	}
	// }

	// 現在は即座にStandbyStateへ遷移
	_ctx.changeState(std::make_unique<StandbyState>(_ctx));
}

void CalibrationState::onExit() {
	_ctx.writeSystemLog("%lu: Exiting CalibrationState", millis());

	// 次の状態（STANDBY）をSDカードに保存
	if (!_ctx.getSDLogger().writeState((int)State::STANDBY)) {
		_ctx.getSerialWriter().log("Failed to write state in SD");
	}

#ifdef USE_FLASH
	// USE_FLAGが定義されている場合はFlashにも保存
	if (!_ctx.getFlashIO().writeState((int)State::STANDBY)) {
		_ctx.getSerialWriter().log("Failed to write state in Flash");
	}
#endif // USE_FLASH
}

State CalibrationState::getState() const {
	return State::CALIBRATION;
}

// BNO055のキャリブレーション完了判定
// system≥1: システム全体が最低限動作可能
// gyro≥3, accel≥3, mag≥3: 各センサーが完全にキャリブレーション完了
bool CalibrationState::isFullyCalibrated(CalibrationStatus calib) {
    return (calib.system >= 1 && calib.gyro >= 3 && calib.accel >= 3 && calib.mag >= 3);
}