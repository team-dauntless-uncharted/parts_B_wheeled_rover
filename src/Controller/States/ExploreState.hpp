/**
 * @file ExploreState.hpp
 */

#pragma once
#include "Controller/ICansatState.hpp"

/**
 * @class ExploreState
 * @brief 探索状態：自律移動と周辺撮影を行う
 */
class ExploreState : public ICansatState {
public:
	/**
	 * @brief コンストラクタ
	 * @param ctx CansatControllerへの参照
	 */
	ExploreState(CansatController& ctx) : _ctx(ctx) {}

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
	 * @return State::EXPLORE
	 */
	State getState() const override;

private:
	CansatController& _ctx;        ///< CansatControllerへの参照
	bool _isInitCamera = false;    ///< カメラが初期化済みかどうか

	/**
	 * @brief PHOTO_MODEでカメラを初期化
	 * @return 成功時true
	 */
	bool setPhotoMode();

	/**
	 * @brief カメラを終了
	 */
	void endPhotoMode();
};
