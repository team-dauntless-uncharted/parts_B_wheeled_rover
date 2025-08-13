#pragma once

class CansatController;

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
	 * 状態終了時に1回だけ呼ぶ処理
	 */
	virtual void onExit() {}
};