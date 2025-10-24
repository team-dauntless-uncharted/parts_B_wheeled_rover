/**
 * @file Speaker.hpp
 * @brief スピーカーを使った音声フィードバック制御
 */

#pragma once

/**
 * @class Speaker
 * @brief 圧電スピーカーを制御して音・メロディーを再生するクラス
 *
 * システムイベント（起動、エラー、状態遷移）に対応して音を鳴らす
 */
class Speaker {
public:
    /**
     * @brief コンストラクタ
     * @param pin スピーカーを制御するピン番号
     *
     * 指定されたピンをOUTPUTモードに設定する
     */
    Speaker(int pin);

    /**
     * @brief 指定周波数・時間で単音を鳴らす
     * @param freq 周波数（Hz）
     * @param duration 音を鳴らす時間（ミリ秒）
     *
     * tone() 関数を使用して指定周波数の音を鳴らす
     * 音が鳴り終わるまで関数はブロックされる
     */
    void beep(int freq, int duration);

    /**
     * @brief 起動時のメロディーを再生する
     *
     * システム起動成功の通知に使用する
     */
    void playStart();

    /**
     * @brief エラー時のメロディーを再生する
     *
     * エラーや異常状態の通知に使用する
     */
    void playError();

    /**
     * @brief 短いビープ音を鳴らす
     *
     * 簡単な通知や確認音として使用する
     */
    void playBeep();

    /**
     * @brief 状態番号に対応したメロディーを再生する
     * @param state 状態番号（ビットフラグとして解釈）
     *
     * stateの各ビットに対応した音階を再生する
     *
     * ビット対応:
     * - bit0: CALIBRATION (ド)
     * - bit1: STANDBY (レ)
     * - bit2: LAUNCH (ミ)
     * - bit3: DROP (ファ)
     * - bit4: ESCAPE (ソ)
     * - bit5: DETECTION (ラ)
     * - bit6: RECORDING (シ)
     * - bit7: EXPLORE (高いド)
     */
    void playState(const int state);

private:
    /**
     * @brief 周波数配列からメロディーを再生する内部メソッド
     * @param melody 周波数配列（Hz単位）
     * @param notes 配列の要素数（音の数）
     * @param noteDuration 各音の長さ（ミリ秒）
     *
     * 配列内の各周波数を順番に再生し、音の間に50msの休止を入れる
     */
    void _play(const float* melody, int notes, int noteDuration);

    int _pin;  ///< スピーカーが接続されているピン番号
};