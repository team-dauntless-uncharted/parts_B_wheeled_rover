/**
 * @file CdSSensor.hpp
 * @brief CdS光センサー（硫化カドミウムセル）の制御
 */

#pragma once

/**
 * @class CdSSensor
 * @brief CdS光センサーで光量を測定するクラス
 *
 * アナログ入力ピンで光量を0-1023の値として読み取る
 */
class CdSSensor {
public:
	/**
	 * @brief コンストラクタ
	 * @param pin CdSセンサーが接続されているアナログピン番号
	 *
	 * 指定されたアナログピンからCdSセンサーの値を読み取れるよう初期化する
	 */
	CdSSensor(int pin);

	/**
	 * @brief 現在の光量を読み取る
	 * @return 光量の値（0-1023）
	 *
	 * analogRead()を使用してセンサー値を取得する
	 * 値が大きいほど明るく、小さいほど暗いことを示す
	 */
	int read();

private:
	int _pin;  ///< CdSセンサーが接続されているアナログピン番号
};