/**
 * @file EscapeState.hpp
 */

#pragma once
#include "Controller/ICansatState.hpp"

/**
 * @class EscapeState
 * @brief 脱出状態：コンテナからの脱出を試みる
 */
class EscapeState : public ICansatState {
public:
	/**
	 * @brief コンストラクタ
	 * @param ctx CansatControllerへの参照
	 */
	EscapeState(CansatController& ctx) : _ctx(ctx) {}

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
	 * @return State::ESCAPE
	 */
	State getState() const override;

private:
	CansatController& _ctx;        ///< CansatControllerへの参照
	unsigned long _startTime;      ///< 脱出開始時刻（タイムアウト判定用）
	int _count;                    ///< 脱出パターンのカウンタ（0-3をローテーション）

	/**
	 * @brief パルス前進：断続的に前進する
	 * @param pwm モーターのPWM値
	 * @param repeat 繰り返し回数
	 */
	void pulseForward(int pwm, int repeat);

	/**
	 * @brief ロッキング脱出：前後左右のロッキング動作
	 * @param pwm モーターのPWM値
	 * @param repeat 繰り返し回数
	 */
	void rockingEscape(int pwm, int repeat);

	/**
	 * @brief 位相脱出：S字カーブのような動作
	 * @param pwm モーターのPWM値
	 * @param repeat 繰り返し回数
	 */
	void phaseEscape(int pwm, int repeat);

	/**
	 * @brief 渦巻き脱出：渦巻き状の動作で周囲を探索
	 * @param pwm モーターのPWM値
	 * @param repeat 繰り返し回数
	 */
	void spiralEscape(int pwm, int repeat);
};