/**
 * @file Speaker.cpp
 * @brief スピーカー制御クラスの実装
 */

#include "Actuator/Speaker/Speaker.hpp"
#include <Arduino.h>

// --- メロディーの定義 (周波数: Hz) ---
const float _start[] = { 261.63, 329.63, 392.00 }; // ド、ミ、ソ（起動音）
const float _error[] = { 440.00, 220.00 };         // ラ(高)、ラ(低)（エラー音）
const float _bell[]  = { 880.00 };                 // ラ(とても高い)（ビープ音）

// 各状態に対応する周波数配列（ビットインデックスに対応）
const float melodies[] = {
    261.63, // bit0: CALIBRATION → ド
    293.66, // bit1: STANDBY     → レ
    329.63, // bit2: LAUNCH      → ミ
    349.23, // bit3: DROP        → ファ
    392.00, // bit4: ESCAPE      → ソ
    440.00, // bit5: DETECTION   → ラ
    493.88, // bit6: RECORDING   → シ
    523.25, // bit7: EXPLORE     → ド（高い）
};

Speaker::Speaker(int pin) : _pin(pin) {
    pinMode(_pin, OUTPUT);
}

void Speaker::beep(int freq, int duration) {
    tone(_pin, freq, duration);
    delay(duration);
}

void Speaker::playStart() {
    _play(_start, sizeof(_start)/sizeof(float), 150);
}

void Speaker::playError() {
    _play(_error, sizeof(_error)/sizeof(float), 300);
}

void Speaker::playBeep() {
    _play(_bell, sizeof(_bell)/sizeof(float), 200);
}

void Speaker::_play(const float* melody, int notes, int noteDuration) {
    for (int i = 0; i < notes; i++) {
        tone(_pin, (unsigned int)melody[i], noteDuration);
        delay(noteDuration + 50); // 音の間に50msの休止を挿入
    }
}

void Speaker::playState(const int state) {
    // stateの各ビットをチェックし、立っているビットに対応する音を鳴らす
    for (int i = 0; i < (int)(sizeof(melodies)/sizeof(float)); i++) {
        if (state & (1 << i)) {
            tone(_pin, (unsigned int)melodies[i], 200);
            delay(250); // 音(200ms) + 休止(50ms)
        }
    }
}