/**
 * @file CalibrationState.hpp
 */

#pragma once
#include "Controller/ICansatState.hpp"

#include <BNO055Library.h>

/**
 * @class CalibrationState
 * @brief キャリブレーション状態：センサーの初期化とキャリブレーションを行う
 *
 * @note キャリブレーション処理はコメントアウトされており、実際には実行していない
 */
class CalibrationState : public ICansatState {
public:
	/**
	 * @brief コンストラクタ
	 * @param ctx CansatControllerへの参照
	 */
	CalibrationState(CansatController& ctx) : _ctx(ctx) {}

	/**
	 * @brief 状態開始時の初期化処理
	 */
	void onEnter() override;

	/**
	 * @brief 状態の更新処理（200msごとに呼ばれる）
	 */
	void onUpdate() override;

	/**
	 * @brief 状態終了時の処理
	 */
	void onExit() override;

	/**
	 * @brief 現在の状態を取得
	 * @return State::CALIBRATION
	 */
	State getState() const override;

private:
	CansatController& _ctx;        ///< CansatControllerへの参照
	unsigned long _startTime;      ///< キャリブレーション開始時刻（タイムアウト判定用）

	/**
	 * @brief BNO055のキャリブレーションが完了しているか判定
	 * @param calib BNO055のキャリブレーションステータス
	 * @return system≥1, gyro≥3, accel≥3, mag≥3の場合true
	 * @note 各センサーのキャリブレーションは0-3の4段階（3が完全）
	 */
	bool isFullyCalibrated(CalibrationStatus calib);
};