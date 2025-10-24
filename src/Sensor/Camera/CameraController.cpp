/**
 * @file CameraController.cpp
 * @brief カメラ制御クラスの実装
 */

#include "Sensor/Camera/CameraController.hpp"
#include <Arduino.h>

CameraController::CameraController() {}
CameraController::~CameraController() {}

bool CameraController::checkCameraError(CamErr err) {
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return false;
    }
    return true;
}

bool CameraController::begin(CameraMode mode) {
    CamErr err;
    switch (mode) {
        case DETECTION_MODE:
            // 物体検知モード: QQVGA (160x120) YUV422形式
            // YUV422はEdge Impulseの画像処理に適した形式
            err = theCamera.begin();
            if (!checkCameraError(err)) return false;

            err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_AUTO);
            if (!checkCameraError(err)) return false;

            err = theCamera.setStillPictureImageFormat(CAM_IMGSIZE_QQVGA_H, CAM_IMGSIZE_QQVGA_V, CAM_IMAGE_PIX_FMT_YUV422);
            if (!checkCameraError(err)) return false;

            break;

        case VIDEO_MODE:
            // 動画録画モード: QVGA (320x240) 30fps JPEG形式
            // バッファ数2、フレームレート30fps、QVGA解像度、バッファ数3
            err = theCamera.begin(2, CAM_VIDEO_FPS_30, CAM_IMGSIZE_QVGA_H, CAM_IMGSIZE_QVGA_V, CAM_IMAGE_PIX_FMT_JPG, 3);
            if (!checkCameraError(err)) return false;

            break;

        case PHOTO_MODE:
            // 静止画撮影モード: HD (1280x720) JPEG形式
            // 高解像度での画像保存用
            err = theCamera.begin();
            if (!checkCameraError(err)) return false;

            err = theCamera.setStillPictureImageFormat(CAM_IMGSIZE_HD_H, CAM_IMGSIZE_HD_V, CAM_IMAGE_PIX_FMT_JPG);
            if (!checkCameraError(err)) return false;

            break;

        default:
            break;
    }

    return true;
}

void CameraController::end() {
    theCamera.end();
}

bool CameraController::startStreaming(bool enable, camera_cb_t cb) {
    CamErr err = theCamera.startStreaming(enable, cb);
    return checkCameraError(err);
}

bool CameraController::takePicture(void** imgBuff, size_t* imgSize) {
    CamImage img = theCamera.takePicture();
    if (img.isAvailable()) {
        *imgBuff = img.getImgBuff();
        *imgSize = img.getImgSize();
        return true;
    }
    return false;
}

void CameraController::printError(enum CamErr err) {
    // カメラエラーコードに応じた人間可読なメッセージを出力
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

/* --- カメラパラメータ設定関数の実装 --- */

bool CameraController::setJPEGQuality(int quality) {
    CamErr err = theCamera.setJPEGQuality(quality);
    return checkCameraError(err);
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
    return checkCameraError(err);
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
    return checkCameraError(err);
}

bool CameraController::setAutoWhiteBalanceMode(CAM_WHITE_BALANCE wb) {
    CamErr err = theCamera.setAutoWhiteBalanceMode(wb);
    return checkCameraError(err);
}

bool CameraController::setAutoWhiteBalance(bool enable) {
    CamErr err = theCamera.setAutoWhiteBalance(enable);
    return checkCameraError(err);
}

bool CameraController::setAutoExposure(bool enable) {
    CamErr err = theCamera.setAutoExposure(enable);
    return checkCameraError(err);
}

bool CameraController::setAbsoluteExposure(int32_t exposure_time) {
    CamErr err = theCamera.setAbsoluteExposure(exposure_time);
    return checkCameraError(err);
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
    return checkCameraError(err);
}

CAM_HDR_MODE CameraController::getHDR() {
    CAM_HDR_MODE mode = theCamera.getHDR();
    // NOTE: カメラが初期化されていない場合でもCAM_HDR_MODE_OFFを返す
    // getFd() < 0はカメラ未初期化を示す
    if (mode == CAM_HDR_MODE_OFF && theCamera.getFd() < 0) {
        return CAM_HDR_MODE_OFF;
    }
    return mode;
}

bool CameraController::setColorEffect(CAM_COLOR_FX effect) {
    CamErr err = theCamera.setColorEffect(effect);
    return checkCameraError(err);
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
    return checkCameraError(err);
}