/**
 * @file ICansatState.hpp
 * @brief CanSat状態マシンの基底インターフェース
 */

#pragma once

class CansatController;

/**
 * @enum State
 * @brief CanSatのミッション状態を表す列挙型
 *
 * 各状態は0から順番に番号が割り当てられ、state.txtファイルに保存される
 * この番号を使って特定の状態から実行を開始する
 */
enum State {
    CALIBRATION = 0,  ///< キャリブレーション状態：センサー初期化
    STANDBY,          ///< スタンバイ状態：発射前待機
    LAUNCH,           ///< 発射検知状態：CdSセンサーで放出検知
    DROP,             ///< 降下状態：Twelite起動、降下監視
	ESCAPE,           ///< 脱出状態：ケースからの脱出
	DETECTION,        ///< 検知状態：A-parts（目標）の物体検知
	RECORDING,        ///< 録画状態：A-parts発射動画の撮影
	EXPLORE,          ///< 探索状態：自律移動と周辺撮影
};

/**
 * @class ICansatState
 * @brief CanSat状態の抽象基底クラス（インターフェース）
 *
 * ステートマシンパターンを実装するための基底クラス
 * 各具体的な状態（CalibrationState, StandbyStateなど）はこのクラスを継承し、
 * onEnter(), onUpdate(), onExit() をオーバーライドする
 *
 * 状態遷移の流れ:
 * 1. 前の状態の onExit() が呼ばれる
 * 2. CansatController::changeState() で状態が切り替わる
 * 3. 新しい状態の onEnter() が呼ばれる（初期化処理）
 * 4. メインループで onUpdate() が周期的に呼ばれる（200msごと）
 * 5. 次の状態に遷移する際、onExit() が呼ばれる（終了処理）
 */
class ICansatState {
public:
	/**
	 * @brief 仮想デストラクタ
	 */
	virtual ~ICansatState() = default;

	/**
	 * @brief 状態開始時に1回だけ呼ばれる処理
	 */
	virtual void onEnter() {}

	/**
	 * @brief 状態のメインループ処理（周期的に呼ばれる）
	 * 
	 * 次の状態に遷移する場合は、この中で changeState() を呼び出す
	 */
	virtual void onUpdate() {}

	/**
	 * @brief 状態終了時に1回だけ呼ばれる処理
	 */
	virtual void onExit() {}

	/**
	 * @brief 現在の状態を返す
	 * @return 状態を表す列挙値
	 */
	virtual State getState() const {}
};