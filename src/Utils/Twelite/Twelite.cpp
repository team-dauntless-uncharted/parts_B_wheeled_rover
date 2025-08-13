#include "Twelite.hpp"

/**
 * @brief Construct a new Twelite Controller:: Twelite Controller object
 */
TweliteController::TweliteController() {
}

/**
 * @brief Tweliteとのシリアル通信(Serial2)を初期化します。
 * @param baud ボーレート（デフォルト: 115200）
 * @return true 常にtrueを返します。
 */
bool TweliteController::begin(unsigned long baud) {
    Serial2.begin(baud);
    while (!Serial2) {
        ; // シリアルポートが接続されるのを待つ
    }
    return true;
}

/**
 * @brief メッセージを送信します。末尾に改行が追加されます。
 * @param message 送信する文字列
 */
void TweliteController::send(const char* message) {
    Serial2.println(message);
}

/**
 * @brief 受信バッファに読み取り可能なデータがあるか確認します。
 * @return 読み取り可能なバイト数
 */
int TweliteController::available() {
    return Serial2.available();
}

/**
 * @brief メッセージを1行受信します。
 * @return 受信した文字列。データがない場合は空文字列。
 */
String TweliteController::receive() {
    if (available() > 0) {
        String message = Serial2.readStringUntil('\n');
        message.trim(); // 前後の空白文字や改行コードを削除
        return message;
    }
    return "";
}