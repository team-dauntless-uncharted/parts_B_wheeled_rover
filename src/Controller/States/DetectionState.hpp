/**
 * @file DetectionState.hpp
 */

#pragma once

#include <Arduino.h>
#include "Controller/ICansatState.hpp"

#define EI_CAMERA_RAW_FRAME_BUFFER_COLS 160  ///< Edge Impulse用の生フレームバッファの幅
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS 120  ///< Edge Impulse用の生フレームバッファの高さ
#define EI_CAMERA_RAW_FRAME_BUFFER_BYTES 2   ///< YUV422のピクセルあたりバイト数

#define OUTPUT_WIDTH 96   ///< ML推論用にリサイズ後の画像幅
#define OUTPUT_HEIGHT 96  ///< ML推論用にリサイズ後の画像高さ

#define ALIGN_PTR(p,a)   ((p & (a-1)) ?(((uintptr_t)p + a) & ~(uintptr_t)(a-1)) : p)  ///< ポインタを指定アラインメントに揃えるマクロ

/**
 * @struct DetectionObject
 * @brief 検出されたオブジェクトの情報
 */
struct DetectionObject {
	float value;     ///< 検出の信頼度（0.0-1.0）
	uint32_t x;      ///< オブジェクトのX座標
	uint32_t y;      ///< オブジェクトのY座標
};

/**
 * @struct DetectionResult
 * @brief 物体検出の結果を格納する構造体
 */
struct DetectionResult {
	bool has_detection;                    ///< 検出があったかどうか
	uint32_t detection_count;              ///< 検出されたオブジェクトの数
	DetectionObject detected_objects[10];  ///< 検出されたオブジェクトの配列（最大10個）
};

/**
 * @class DetectionState
 * @brief 検知状態：A-parts物体検知を行う
 */
class DetectionState : public ICansatState {
public:
	/**
	 * @brief コンストラクタ
	 * @param ctx CansatControllerへの参照
	 */
	DetectionState(CansatController& ctx) : _ctx(ctx), _result() {}

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
	 * @return State::DETECTION
	 */
	State getState() const override;

private:
	CansatController& _ctx;           ///< CansatControllerへの参照
	int _failedCount = 0;             ///< 検出失敗回数のカウンタ
	bool _isInitCamera = false;       ///< カメラが初期化済みかどうか
	bool _isInitEdgeImpulse = false;  ///< Edge Impulseが初期化済みかどうか
	DetectionResult _result;          ///< 物体検出の結果
	int _succeedCount = 0;            ///< 検出成功回数のカウンタ

	/**
	 * @brief DETECTION_MODEでカメラを初期化
	 * @return 成功時true
	 */
	bool setDetectionMode();

	/**
	 * @brief カメラを終了
	 */
	void endDetectionMode();

	static uint8_t *_image_buffer_base;      ///< Edge Impulse用の画像バッファ（ベースアドレス）
	static uint8_t *_current_image_buffer;   ///< Edge Impulse用の画像バッファ（アラインメント済み）

	/**
	 * @brief Edge Impulseの推論用にピクセルデータを取得
	 * @param offset ピクセルオフセット
	 * @param length 取得するピクセル数
	 * @param out_ptr 出力先のfloat配列
	 * @return 常に0
	 */
	static int get_image_data(size_t offset, size_t length, float *out_ptr);

	/**
	 * @brief Edge Impulseの初期化（画像バッファ確保）
	 * @return 成功時true
	 */
	bool beginEdgeImpulse(void);

	/**
	 * @brief Edge Impulseの終了（画像バッファ解放）
	 */
	void endEdgeImpulse(void);

	/**
	 * @brief YUV422形式をRGB888形式に変換
	 * @param yuv_buffer YUV422画像データ
	 * @param yuv_size YUV422データのサイズ
	 * @return 成功時true
	 */
	bool convertYUV422ToRGB888(const uint8_t *yuv_buffer, size_t yuv_size);

	/**
	 * @brief 画像を96x96にリサイズ
	 * @return 常にtrue
	 */
	bool resizeImage(void);

	/**
	 * @brief Edge Impulseで物体検出を実行
	 * @return 成功時true
	 */
	bool detectObjects(void);
};
