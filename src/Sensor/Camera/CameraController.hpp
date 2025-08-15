/**
 * @file CameraController.hpp
 * @brief カメラの制御を行うクラスの定義
 */

#pragma once
#include <Camera.h>

enum CameraMode {
    DETECTION_MODE = 0,
    VIDEO_MODE = 1,
    EXPLORE_MODE = 2,
};

/**
 * @brief カメラの制御を行うクラス
 */
class CameraController {
public:
    CameraController();
    ~CameraController();

    /**
     * @brief カメラの初期化
     * @param mode カメラのモード
     * @return true: 成功, false: 失敗
     */
    bool begin(CameraMode mode);

    /**
     * @brief カメラの終了
     */
    void end();

    /**
     * @brief ストリーミングの開始
     * @return true: 成功, false: 失敗
     */
    bool startStreaming(bool enable, camera_cb_t cb = NULL);

    /**
     * @brief 写真の撮影
     * @param imgBuff 写真のバッファ
     * @param imgSize 写真のサイズ
     * @return true: 成功, false: 失敗
     */
    bool takePicture(void** imgBuff, size_t* imgSize);

    /**
     * @brief エラーの出力
     * @param err エラーの種類
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
     * @brief 静止画解像度の設定
     * @param width 幅 (CAM_IMGSIZE_*_H)
     * @param height 高さ (CAM_IMGSIZE_*_V)
     * @param format ピクセルフォーマット (CAM_IMAGE_PIX_FMT_*)
     * @return true: 成功, false: 失敗
     */
    bool setStillPictureImageFormat(int width, int height, CAM_IMAGE_PIX_FMT format);

private:
    bool checkCameraError(CamErr err);
};