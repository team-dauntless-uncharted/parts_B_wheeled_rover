#include "Actuator/Speaker/Speaker.hpp"
#include <Arduino.h>

// --- メロディーの定義 (周波数: Hz) ---
const float _start[] = { 261.63, 329.63, 392.00 }; // ド、ミ、ソ
const float _error[] = { 440.00, 220.00 };       // ラ(高い)、ラ(低い)
const float _bell[]  = { 880.00 };               // ラ(とても高い)

/**
 * @brief Speakerクラスのコンストラクタ
 * @param pin スピーカーが接続されているピン番号
 */
Speaker::Speaker(int pin) : _pin(pin) {
    pinMode(_pin, OUTPUT);
}

/**
 * @brief 指定された周波数と時間で単一の音を鳴らす
 * @param freq 周波数 (Hz)
 * @param duration 音を鳴らす時間 (ms)
 */
void Speaker::beep(int freq, int duration) {
    tone(_pin, freq, duration);
    delay(duration); // 音が鳴り終わるまで待機
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

/**
 * @brief 配列で定義されたメロディーを再生する（プライベートメソッド）
 * @param melody 周波数の配列
 * @param notes 配列の要素数（音の数）
 * @param noteDuration 各音の長さ (ms)
 */
void Speaker::_play(const float* melody, int notes, int noteDuration) {
    for (int i = 0; i < notes; i++) {
        tone(_pin, (unsigned int)melody[i], noteDuration);
        delay(noteDuration + 50); // 音の長さ + 音の間の短い休止
    }
}