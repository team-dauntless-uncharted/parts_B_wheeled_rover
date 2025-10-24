/**
 * @file SerialWriter.cpp
 * @brief シリアルログ出力クラスの実装
 */

#include "Utils/Serial/SerialWriter.hpp"
#include <cstdarg>

SerialWriter::SerialWriter() {
}

bool SerialWriter::begin(unsigned long baud) {
    Serial.begin(baud);

    // シリアルモニタが開かれるまでここでブロックされる
    while (!Serial) {
        ;
    }

    return true;
}

void SerialWriter::log(const char* message) {
    Serial.println(message);
}

void SerialWriter::logf(const char* fmt, ...) {
    char buffer[256];  // フォーマット済み文字列バッファ（最大256バイト）

    // 可変長引数をフォーマット文字列に従って展開
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    Serial.println(buffer);
}