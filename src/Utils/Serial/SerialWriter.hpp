/**
 * @file SerialWriter.hpp
 * @brief シリアル通信によるデバッグログ出力クラス
 */

#pragma once
#include <Arduino.h>

/**
 * @class SerialWriter
 * @brief シリアル通信を使ったログ出力を管理するクラス
 *
 * Arduino の Serial を使用してデバッグメッセージをUSB経由で出力する
 */
class SerialWriter {
public:
	/**
	 * @brief コンストラクタ
	 */
	SerialWriter();

	/**
	 * @brief シリアル通信を初期化する
	 * @param baud ボーレート（デフォルト: 115200）
	 * @return true: 成功（常にtrue）
	 */
	bool begin(unsigned long baud = 115200);

	/**
	 * @brief メッセージをシリアル出力する
	 * @param message 出力する文字列
	 *
	 * Serial.println()を使用してメッセージを出力し、改行を追加する
	 */
	void log(const char* message);

	/**
	 * @brief フォーマット指定でメッセージをシリアル出力する
	 * @param fmt printf形式のフォーマット文字列
	 * @param ... 可変長引数（フォーマット指定子に対応する値）
	 *
	 * printf形式でフォーマットされたメッセージを出力する
	 * 内部バッファサイズは256バイト
	 *
	 * 使用例:
	 * @code
	 * writer.logf("Sensor value: %d, Temperature: %.2f", sensorValue, temp);
	 * @endcode
	 */
	void logf(const char* fmt, ...);
};