/**
 * @file CdSSensor.cpp
 * @brief CdS光センサー制御クラスの実装
 */

#include <Arduino.h>
#include "Sensor/CdS/CdSSensor.hpp"

CdSSensor::CdSSensor(int pin) : _pin(pin) {
    // NOTE: アナログピンはpinMode()の設定不要
    // analogRead()を呼ぶだけで自動的にアナログ入力として動作する
}

int CdSSensor::read() {
    // アナログ値（0-1023）を返す
    // 値が大きい = 明るい、値が小さい = 暗い
    return analogRead(_pin);
}