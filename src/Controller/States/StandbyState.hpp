/**
 * @file StandbyState.hpp
 */

#pragma once
#include "Controller/ICansatState.hpp"

/**
 * @class StandbyState
 * @brief スタンバイ状態：発射前の待機状態
 */
class StandbyState : public ICansatState {
public:
	/**
	 * @brief コンストラクタ
	 * @param ctx CansatControllerへの参照
	 */
	StandbyState(CansatController& ctx) : _ctx(ctx) {}

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
	 * @return State::STANDBY
	 */
	State getState() const override;

private:
	CansatController& _ctx;        ///< CansatControllerへの参照
	unsigned long _startTime;      ///< 待機開始時刻（タイムアウト判定用）
};