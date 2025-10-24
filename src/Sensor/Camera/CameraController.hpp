/**
 * @file CameraController.hpp
 * @brief カメラの制御を行うクラスの定義
 */

#pragma once
#include <Camera.h>

/**
 * @enum CameraMode
 * @brief カメラの動作モード
 *
 * 用途に応じてカメラの解像度・フォーマット・FPSを切り替える
 */
enum CameraMode {
    DETECTION_MODE = 0,  ///< 物体検知モード: QQVGA (160x120) YUV422形式
    VIDEO_MODE = 1,      ///< 動画録画モード: QVGA (320x240) 30fps JPEG形式
    PHOTO_MODE = 2,      ///< 静止画撮影モード: HD (1280x720) JPEG形式
};

/**
 * @class CameraController
 * @brief Spresense内蔵カメラを制御するクラス
 *
 * Sony Spresenseの内蔵カメラ（theCamera）をラップし、
 * モード切替、撮影、各種パラメータ設定を行う
 * 3つのモード（物体検知、動画録画、静止画撮影）を提供する
 */
class CameraController {
public:
    /**
     * @brief コンストラクタ
     */
    CameraController();

    /**
     * @brief デストラクタ
     */
    ~CameraController();

    /**
     * @brief カメラの初期化
     * @param mode カメラのモード（DETECTION_MODE/VIDEO_MODE/PHOTO_MODE）
     * @return true: 成功, false: 失敗
     *
     * モードに応じて以下の設定を行います:
     * - DETECTION_MODE: QQVGA (160x120) YUV422、オートホワイトバランス有効
     * - VIDEO_MODE: QVGA (320x240) 30fps JPEG、バッファ数3
     * - PHOTO_MODE: HD (1280x720) JPEG
     */
    bool begin(CameraMode mode);

    /**
     * @brief カメラの終了
     *
     * カメラデバイスを解放し、リソースを解放する
     */
    void end();

    /**
     * @brief ストリーミングの開始・停止
     * @param enable true: 開始, false: 停止
     * @param cb コールバック関数（オプション）
     * @return true: 成功, false: 失敗
     *
     * 動画録画モード時にストリーミングを制御する
     * コールバック関数を指定すると、フレーム取得時に呼び出される
     */
    bool startStreaming(bool enable, camera_cb_t cb = NULL);

    /**
     * @brief 写真を撮影する
     * @param imgBuff 撮影した画像データのバッファポインタを格納する変数へのポインタ
     * @param imgSize 画像データのサイズを格納する変数へのポインタ
     * @return true: 成功, false: 失敗
     *
     * takePicture()を実行し、画像が利用可能であれば
     * imgBuffとimgSizeに画像データの情報を設定する
     */
    bool takePicture(void** imgBuff, size_t* imgSize);

    /**
     * @brief カメラエラーの詳細をシリアル出力する
     * @param err カメラエラーコード
     *
     * エラー種別に応じた人間可読なメッセージをSerial.printで出力する
     */
    void printError(enum CamErr err);

    // カメラパラメータ設定関数
    /**
     * @brief JPEG品質の設定
     * @param quality JPEG品質 (1-100)
     * @return true: 成功, false: 失敗
     */
    bool setJPEGQuality(int quality);

    /**
     * @brief JPEG品質の取得
     * @return JPEG品質 (1-100) または -1 (エラー)
     */
    int getJPEGQuality();

    /**
     * @brief ISO感度の設定
     * @param iso_sense ISO感度値 (CAM_ISO_SENSITIVITY_*)
     * @return true: 成功, false: 失敗
     */
    bool setISOSensitivity(int iso_sense);

    /**
     * @brief ISO感度の取得
     * @return ISO感度値 または -1 (エラー)
     */
    int getISOSensitivity();

    /**
     * @brief 自動ISO感度の設定
     * @param enable true: 有効, false: 無効
     * @return true: 成功, false: 失敗
     */
    bool setAutoISOSensitivity(bool enable);

    /**
     * @brief ホワイトバランスモードの設定
     * @param wb ホワイトバランスモード (CAM_WHITE_BALANCE_*)
     * @return true: 成功, false: 失敗
     */
    bool setAutoWhiteBalanceMode(CAM_WHITE_BALANCE wb);

    /**
     * @brief 自動ホワイトバランスの設定
     * @param enable true: 有効, false: 無効
     * @return true: 成功, false: 失敗
     */
    bool setAutoWhiteBalance(bool enable);

    /**
     * @brief 自動露光の設定
     * @param enable true: 有効, false: 無効
     * @return true: 成功, false: 失敗
     */
    bool setAutoExposure(bool enable);

    /**
     * @brief 露光時間の設定
     * @param exposure_time 露光時間 (100usec単位)
     * @return true: 成功, false: 失敗
     */
    bool setAbsoluteExposure(int32_t exposure_time);

    /**
     * @brief 露光時間の取得
     * @return 露光時間 (100usec単位) または -1 (エラー)
     */
    int32_t getAbsoluteExposure();

    /**
     * @brief HDRモードの設定
     * @param mode HDRモード (CAM_HDR_MODE_*)
     * @return true: 成功, false: 失敗
     */
    bool setHDR(CAM_HDR_MODE mode);

    /**
     * @brief HDRモードの取得
     * @return HDRモード または CAM_HDR_MODE_OFF (エラー)
     */
    CAM_HDR_MODE getHDR();

    /**
     * @brief 色効果の設定
     * @param effect 色効果 (CAM_COLOR_FX_*)
     * @return true: 成功, false: 失敗
     */
    bool setColorEffect(CAM_COLOR_FX effect);

    /**
     * @brief フレーム間隔の取得
     * @return フレーム間隔 (100usec単位) または -1 (エラー)
     */
    int getFrameInterval();

    /**
     * @brief カメラデバイスタイプの取得
     * @return カメラデバイスタイプ
     */
    CAM_DEVICE_TYPE getDeviceType();

    // 解像度・フレームレート設定関数
    /**
     * @brief 静止画解像度・フォーマットの設定
     * @param width 幅（CAM_IMGSIZE_*_H定数を使用）
     * @param height 高さ（CAM_IMGSIZE_*_V定数を使用）
     * @param format ピクセルフォーマット（CAM_IMAGE_PIX_FMT_JPG/YUV422など）
     * @return true: 成功, false: 失敗
     *
     * 静止画撮影時の解像度とピクセルフォーマットを設定する
     * YUV422は物体検知用、JPEGは保存用として使い分ける
     */
    bool setStillPictureImageFormat(int width, int height, CAM_IMAGE_PIX_FMT format);

private:
    /**
     * @brief カメラエラーチェック
     * @param err カメラエラーコード
     * @return true: エラーなし, false: エラーあり
     *
     * エラーが発生している場合、printError()でエラー内容を出力する
     */
    bool checkCameraError(CamErr err);
};