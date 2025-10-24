/**
 * @file RecordingState.hpp
 */

#pragma once
#include "Controller/ICansatState.hpp"
#include <Camera.h>

/**
 * @class RecordingState
 * @brief 録画状態：A-parts発射の動画を撮影する
 */
class RecordingState : public ICansatState {
public:
	/**
	 * @brief コンストラクタ
	 * @param ctx CansatControllerへの参照
	 */
	RecordingState(CansatController& ctx) : _ctx(ctx) {}

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
	 * @return State::RECORDING
	 */
	State getState() const override;

	/**
	 * @brief カメラコールバック関数（静的メソッド）
	 * @param img カメラ画像
	 */
	static void CamCB(CamImage img);

private:
	CansatController& _ctx;             ///< CansatControllerへの参照
	unsigned long _startTime;           ///< 録画開始時刻（タイムアウト判定用）
	bool _isInitCamera = false;         ///< カメラが初期化済みかどうか
	static RecordingState *_instance;   ///< 静的コールバック用のインスタンスポインタ
	bool _isRecordingOK = false;        ///< 録画開始可能フラグ

	// エラーハンドリング用メンバ変数
	bool _recordingError = false;       ///< 録画エラーフラグ
	bool _aviRecordingActive = false;   ///< AVI録画がアクティブかどうか
	uint32_t _frameCount = 0;           ///< 録画されたフレーム数

	/**
	 * @brief VIDEO_MODEでカメラを初期化
	 * @return 成功時true
	 */
	bool setRecordingMode();

	/**
	 * @brief AVI録画を実行
	 * @param time_ms 録画時間（ミリ秒）
	 * @return 成功時true
	 */
	bool record(int time_ms);

	/**
	 * @brief カメラを終了
	 */
	void endRecordingMode();

	/**
	 * @brief カメラ画像を処理してAVIファイルに追加
	 * @param img カメラ画像
	 */
	void handleCameraImage(CamImage img);
};
