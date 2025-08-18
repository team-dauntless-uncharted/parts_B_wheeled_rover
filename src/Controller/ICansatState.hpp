#pragma once

class CansatController;

enum State {
    CALIBRATION = 0,
    STANDBY,
    LAUNCH,
    DROP,
    LANDING,
	DETECTION,
	RECORDING,
	EXPLORE,
    NAVIGATION,
    GOAL,
};

class ICansatState {
public:
	virtual ~ICansatState() = default;

	/**
	 * @brief 状態開始時に1回だけ呼ぶ処理
	 */
	virtual void onEnter() {}

	/**
	 * @brief 状態のメインループ処理(周期的に呼ばれる)
	 */
	virtual void onUpdate() {}

	/**
	 * @brief 状態終了時に1回だけ呼ぶ処理
	 */
	virtual void onExit() {}

	/**
	 * @brief 現在の状態を返す
	 */
	virtual State getState() const {}
};