/**
 * @file Led.hpp
 * @brief SPRESENSE内蔵LEDの制御を行う
 */

#pragma once

/**
 * @class Led
 * @brief LEDの点灯・消灯・トグル制御を行うクラス
 *
 * SPRESENSE内蔵のLED（LED0-LED3）やその他のデジタルピンに接続されたLEDを制御する
 * 各LEDの状態を保持し、簡単なインターフェースで操作する
 */
class Led {
public:
	/**
	 * @brief コンストラクタ
	 * @param pin 制御するLEDが接続されているピン番号
	 *
	 * 指定されたピンをOUTPUTモードに設定し、初期状態をLOW（消灯）にする
	 */
	Led(int pin);

	/**
	 * @brief LEDを点灯する
	 *
	 * ピンをHIGHにしてLEDを点灯状態にする
	 */
	void on();

	/**
	 * @brief LEDを消灯する
	 *
	 * ピンをLOWにしてLEDを消灯状態にする
	 */
	void off();

	/**
	 * @brief LEDの状態を反転する
	 *
	 * 現在の状態（点灯/消灯）を反転します。
	 */
	void toggle();

private:
	int _pin;     ///< LEDが接続されているピン番号
	bool _state;  ///< LEDの現在の状態（true: 点灯, false: 消灯）
};