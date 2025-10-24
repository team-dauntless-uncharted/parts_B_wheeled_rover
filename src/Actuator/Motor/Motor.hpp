/**
 * @file Motor.hpp
 * @brief 左右のタイヤを動作するためのモータの制御を行う
 */

#pragma once

/**
 * @class Motor
 * @brief 左右2輪のサーボモータを制御するクラス
 *
 * 各モータは3本のピン（方向制御2本 + PWM速度制御1本）で制御する
 * 前進、後退、左右旋回、片輪制御など、基本的な移動パターンを提供する
 */
class Motor {
public:
	/**
	 * @brief コンストラクタ
	 * @param pinR 右モータ制御ピン配列 [0]:方向1, [1]:方向2, [2]:PWM速度
	 * @param pinL 左モータ制御ピン配列 [0]:方向1, [1]:方向2, [2]:PWM速度
	 *
	 * 指定された6本のピンをOUTPUTモードに設定する
	 * ピン配列は3要素で、[0][1]が方向制御、[2]がPWM速度制御
	 */
	Motor(int pinR[3], int pinL[3]);

	/**
	 * @brief 前進する
	 * @param pwm モータ速度（0-255）
	 *
	 * 左右両輪を同じ速度で前方向に回転させる
	 */
	void forward(int pwm);

	/**
	 * @brief 後退する
	 * @param pwm モータ速度（0-255）
	 *
	 * 左右両輪を同じ速度で後方向に回転させる
	 */
	void backward(int pwm);

	/**
	 * @brief 右旋回する（信地旋回）
	 * @param pwm モータ速度（0-255）
	 *
	 * 右輪を前進、左輪を後退させて、その場で右に旋回する
	 */
	void turnRight(int pwm);

	/**
	 * @brief 左旋回する（信地旋回）
	 * @param pwm モータ速度（0-255）
	 *
	 * 左輪を前進、右輪を後退させて、その場で左に旋回する
	 */
	void turnLeft(int pwm);

	/**
	 * @brief 両輪を停止する
	 *
	 * 左右両方のモータを停止状態（PWM=0）にする
	 */
	void stop();

	/**
	 * @brief 右輪のみ前進する
	 * @param pwm モータ速度（0-255）
	 *
	 * 右輪だけを前方向に回転させる。左輪は現在の状態を維持する
	 */
	void rightForward(int pwm);

	/**
	 * @brief 左輪のみ前進する
	 * @param pwm モータ速度（0-255）
	 *
	 * 左輪だけを前方向に回転させる。右輪は現在の状態を維持する
	 */
	void leftForward(int pwm);

	/**
	 * @brief 右輪のみ後退する
	 * @param pwm モータ速度（0-255）
	 *
	 * 右輪だけを後方向に回転させる。左輪は現在の状態を維持する
	 */
	void rightBackward(int pwm);

	/**
	 * @brief 左輪のみ後退する
	 * @param pwm モータ速度（0-255）
	 *
	 * 左輪だけを後方向に回転させる。右輪は現在の状態を維持する
	 */
	void leftBackward(int pwm);

	/**
	 * @brief 右輪のみ停止する
	 *
	 * 右輪を停止状態（PWM=0）にする。左輪は現在の状態を維持する
	 */
	void rightStop();

	/**
	 * @brief 左輪のみ停止する
	 *
	 * 左輪を停止状態（PWM=0）にする。右輪は現在の状態を維持する
	 */
	void leftStop();

	/**
	 * @brief 蛇行しながら前進する
	 * @param pwm 基準モータ速度（0-255）
	 * @param duration 動作時間（ミリ秒）
	 * @param frequency 蛇行の周波数（Hz）
	 *
	 * 正弦波パターンで左右のモータ速度を変化させ、蛇行しながら前進する
	 * 左右のPWM値は基準値の±30%の範囲で変動する
	 * 指定時間経過後、自動的に停止する
	 */
	void snakeForwardSmooth(int pwm, int duration, int frequency);

private:
	int _motorR[3];  ///< 右モータ制御ピン [0]:方向1, [1]:方向2, [2]:PWM速度
	int _motorL[3];  ///< 左モータ制御ピン [0]:方向1, [1]:方向2, [2]:PWM速度
};