/**
 * @file CameraController.cpp
 * @brief カメラの制御を行うクラスの実装
 */

#include "CameraController.hpp"
#include <Arduino.h>

CameraController::CameraController() {}
CameraController::~CameraController() {}

/**
 * @brief カメラの初期化
 * @return true: 成功, false: 失敗
 */
bool CameraController::begin() {
    CamErr err = theCamera.begin();
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}

/**
 * @brief カメラの終了
 */
void CameraController::end() {
    theCamera.end();
}

/**
 * @brief ストリーミングの開始
 * @return true: 成功, false: 失敗
 */
bool CameraController::startStreaming() {
    CamErr err = theCamera.startStreaming(true, nullptr); // コールバックは未使用
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}

/**
 * @brief 写真の撮影
 * @param pictureIndex 写真のインデックス
 * @param imgBuff 写真のバッファ
 * @param imgSize 写真のサイズ
 * @return true: 成功, false: 失敗
 */
bool CameraController::takePicture(void** imgBuff, size_t* imgSize) {
    CamImage img = theCamera.takePicture();
    if (img.isAvailable()) {
        *imgBuff = img.getImgBuff();
        *imgSize = img.getImgSize();
        return true;
    }
    return false;
}

/**
 * @brief エラーの出力
 * @param err エラーの種類
 */
void CameraController::printError(enum CamErr err) {
    Serial.print("Error: ");
    switch (err) {
        case CAM_ERR_NO_DEVICE:
            Serial.println("No Device");
            break;
        case CAM_ERR_ILLEGAL_DEVERR:
            Serial.println("Illegal device error");
            break;
        case CAM_ERR_ALREADY_INITIALIZED:
            Serial.println("Already initialized");
            break;
        case CAM_ERR_NOT_INITIALIZED:
            Serial.println("Not initialized");
            break;
        case CAM_ERR_NOT_STILL_INITIALIZED:
            Serial.println("Still picture not initialized");
            break;
        case CAM_ERR_CANT_CREATE_THREAD:
            Serial.println("Failed to create thread");
            break;
        case CAM_ERR_INVALID_PARAM:
            Serial.println("Invalid parameter");
            break;
        case CAM_ERR_NO_MEMORY:
            Serial.println("No memory");
            break;
        case CAM_ERR_USR_INUSED:
            Serial.println("Buffer already in use");
            break;
        case CAM_ERR_NOT_PERMITTED:
            Serial.println("Operation not permitted");
            break;
        default:
            break;
    }
}

// カメラパラメータ設定関数の実装
bool CameraController::setJPEGQuality(int quality) {
    CamErr err = theCamera.setJPEGQuality(quality);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}

int CameraController::getJPEGQuality() {
    int quality = theCamera.getJPEGQuality();
    if (quality < 0) {
        printError((CamErr)quality);
        return -1;
    }
    return quality;
}

bool CameraController::setISOSensitivity(int iso_sense) {
    CamErr err = theCamera.setISOSensitivity(iso_sense);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}

int CameraController::getISOSensitivity() {
    int iso = theCamera.getISOSensitivity();
    if (iso < 0) {
        printError((CamErr)iso);
        return -1;
    }
    return iso;
}

bool CameraController::setAutoISOSensitivity(bool enable) {
    CamErr err = theCamera.setAutoISOSensitivity(enable);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}

bool CameraController::setAutoWhiteBalanceMode(CAM_WHITE_BALANCE wb) {
    CamErr err = theCamera.setAutoWhiteBalanceMode(wb);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}

bool CameraController::setAutoWhiteBalance(bool enable) {
    CamErr err = theCamera.setAutoWhiteBalance(enable);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}

bool CameraController::setAutoExposure(bool enable) {
    CamErr err = theCamera.setAutoExposure(enable);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}

bool CameraController::setAbsoluteExposure(int32_t exposure_time) {
    CamErr err = theCamera.setAbsoluteExposure(exposure_time);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}

int32_t CameraController::getAbsoluteExposure() {
    int32_t exposure = theCamera.getAbsoluteExposure();
    if (exposure < 0) {
        printError((CamErr)exposure);
        return -1;
    }
    return exposure;
}

bool CameraController::setHDR(CAM_HDR_MODE mode) {
    CamErr err = theCamera.setHDR(mode);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}

CAM_HDR_MODE CameraController::getHDR() {
    CAM_HDR_MODE mode = theCamera.getHDR();
    if (mode == CAM_HDR_MODE_OFF && theCamera.getFd() < 0) {
        // カメラが初期化されていない場合のエラーハンドリング
        return CAM_HDR_MODE_OFF;
    }
    return mode;
}

bool CameraController::setColorEffect(CAM_COLOR_FX effect) {
    CamErr err = theCamera.setColorEffect(effect);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}

int CameraController::getFrameInterval() {
    int interval = theCamera.getFrameInterval();
    if (interval < 0) {
        printError((CamErr)interval);
        return -1;
    }
    return interval;
}

CAM_DEVICE_TYPE CameraController::getDeviceType() {
    return theCamera.getDeviceType();
}

// 解像度・フレームレート設定関数の実装
bool CameraController::setStillPictureImageFormat(int width, int height, CAM_IMAGE_PIX_FMT format) {
    CamErr err = theCamera.setStillPictureImageFormat(width, height, format);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}