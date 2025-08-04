/**
 * @file TweliteController.cpp
 * @brief Twelite無線モジュールとの通信を管理するクラスの実装
 */
#include "TweliteController.hpp"

/**
 * @brief コンストラクタ
 */
TweliteController::TweliteController() {
    // 現時点では特に処理なし
}

/**
 * @brief Serial2通信を初期化します。
 * @param baudRate 通信速度（ボーレート）
 */
void TweliteController::begin(long baudRate) {
    Serial2.begin(baudRate);
    // モジュールが通信可能になるまで少し待つ
    delay(100);
}

/**
 * @brief メッセージをTweliteモジュール経由で送信します。
 * @param message 送信する文字列
 */
void TweliteController::sendMessage(const char* message) {
    Serial2.println(message);
}

/**
 * @brief Tweliteモジュールからメッセージを受信します。
 * @return 受信した文字列。データがない場合は空文字列を返します。
 */
String TweliteController::receiveMessage() {
    // Serial.available() は0以上の値を返すため、< 0 の条件は常に偽でした。
    // データが利用可能になるまで待機するように修正します。
    while (Serial2.available() == 0) {
        delay(1); // CPUを占有しないように短い待機を入れる
    }
    return Serial2.readStringUntil('\n');
}

/**
 * @brief Tweliteモジュールから1バイトのコマンドを受信します。データが来るまで待機します。
 * @return 受信したコマンド (0-255)。
 */
int TweliteController::receiveCommand() {
    while (Serial2.available() == 0) {
        delay(1); // データが来るまで待機
    }
    return Serial2.read();
}
