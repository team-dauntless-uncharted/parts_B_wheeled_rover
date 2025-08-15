#include "Utils/Serial/SerialWriter.hpp"
#include <cstdarg>

SerialWriter::SerialWriter() {
}

bool SerialWriter::begin(unsigned long baud) {
    Serial.begin(baud);
    while (!Serial) {
        ; // シリアルポートが接続されるのを待つ
    }
    return true;
}

void SerialWriter::log(const char* message) {
    Serial.println(message);
}

void SerialWriter::logf(const char* fmt, ...) {
	char buffer[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);
	Serial.println(buffer);
}