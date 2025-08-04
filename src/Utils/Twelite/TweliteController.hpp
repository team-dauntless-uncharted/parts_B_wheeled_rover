/**
 * @file TweliteController.hpp
 * @brief Twelite無線モジュールとの通信を管理するクラスの定義
 */
#pragma once

#include <Arduino.h>

/**
 * @class TweliteController
 * @brief Twelite無線モジュールとの通信を管理するクラス
 */
class TweliteController {
public:
    /**
     * @brief コンストラクタ
     */
    TweliteController();

    /**
     * @brief Serial2通信を初期化します。
     * @param baudRate 通信速度（ボーレート）。デフォルトは115200。
     */
    void begin(long baudRate = 115200);

    /**
     * @brief メッセージをTweliteモジュール経由で送信します。
     * @param message 送信する文字列
     */
    void sendMessage(const char* message);

    /**
     * @brief Tweliteモジュールからメッセージを受信します。
     * @return 受信した文字列。データがない場合は空文字列を返します。
     */
    String receiveMessage();

    /**
     * @brief Tweliteモジュールから1バイトのコマンドを受信します。データが来るまで待機します。
     * @return 受信したコマンド (0-255)。
     */
    int receiveCommand();
};