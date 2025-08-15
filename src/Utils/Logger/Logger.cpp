#include "Utils/Logger/Logger.hpp"
#include <Arduino.h>

Logger::Logger() : _sd(), _myFile() {}

// 初期化
bool Logger::begin(String csvHeader) {
    if (!sdInit()) {
        return false;
    }
    
    if (!createLogFile(csvHeader)) {
        return false;
    }

    refreshJPEGFileNameIndex();
    refreshPPMFileNameIndex();
    
    return true;
}

// ログの追加
bool Logger::appendLog(const char* message) {
    _myFile = _sd.open("/log.csv", FILE_WRITE);
    
    if (!_myFile) {
        return false;
    }
    
    _myFile.println(message);
    _myFile.close();
    
    return true;
}

bool Logger::sdInit() {
    if (!_sd.begin()) {
        return false;
    }
    return true;
}

bool Logger::createLogFile(String header) {
    _myFile = _sd.open("/log.csv", FILE_WRITE);
    if (!_myFile) {
        return false;
    }
    _myFile.println(header);
    _myFile.close();
    return true;
}

// JPEGファイルの保存
bool Logger::saveJPEGImage(void* buff, size_t size) {
    _myFile = _sd.open(_jpegFileName, FILE_WRITE);
    if (!_myFile) {
        return false;
    }
    _myFile.write((uint8_t*)buff, size);
    _myFile.close();

    shiftJPEGFileName();
    return true;
}

//_jpegFileNameCountの開始番号を決める
void Logger::refreshJPEGFileNameIndex() {
    while (true) {
        shiftJPEGFileName();
        if (!_sd.exists(_jpegFileNameCount)) {
            break;
        }
    }
}

// JPEGファイルのインクリメント
void Logger::shiftJPEGFileName() {
    sprintf(_jpegFileName, "/explore_%04d.jpg", _jpegFileNameCount);
    _jpegFileNameCount++;
}

// TODO: PPMファイルの保存
bool Logger::savePPMImage(void* buff, size_t size) {
    _myFile = _sd.open(_ppmFileName, FILE_WRITE);
    if (!_myFile) {
        return false;
    }

    _myFile.printf("P6\n%lu %lu\n255\n", 96, 96);

    _myFile.write((uint8_t*)buff, size);
    _myFile.close();
    
    shiftPPMFileName();
    return true;
}

// TODO: _ppmFileNameCountの開始番号を決める
void Logger::refreshPPMFileNameIndex() {
    while (true) {
        shiftPPMFileName();
        if (!_sd.exists(_ppmFileNameCount)) {
            break;
        }
    }
}

// TODO: PPMファイルのインクリメント
void Logger::shiftPPMFileName() {
    sprintf(_ppmFileName, "/detection_%4d.ppm", _ppmFileNameCount);
    _ppmFileNameCount++;
}