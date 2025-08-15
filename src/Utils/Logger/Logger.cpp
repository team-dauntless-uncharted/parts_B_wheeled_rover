#include "Utils/Logger/Logger.hpp"
#include <Arduino.h>

Logger::Logger() : _sd() {}

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
    File csvFile = _sd.open("/log.csv", FILE_WRITE);
    
    if (!csvFile) {
        return false;
    }
    
    csvFile.println(message);
    csvFile.close();
    
    return true;
}

bool Logger::sdInit() {
    if (!_sd.begin()) {
        return false;
    }
    return true;
}

bool Logger::createLogFile(String header) {
    File csvFile = _sd.open("/log.csv", FILE_WRITE);
    if (!csvFile) {
        return false;
    }

    csvFile.println(header);
    csvFile.close();

    return true;
}

// JPEGファイルの保存
bool Logger::saveJPEGImage(void* buff, size_t size) {
    File jpegFile = _sd.open(_jpegFileName, FILE_WRITE);
    if (!jpegFile) {
        return false;
    }
    jpegFile.write((uint8_t*)buff, size);
    jpegFile.close();

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
    File ppmFile = _sd.open(_ppmFileName, FILE_WRITE);
    if (!ppmFile) {
        return false;
    }

    ppmFile.printf("P6\n%lu %lu\n255\n", 96, 96);

    ppmFile.write((uint8_t*)buff, size);
    ppmFile.close();
    
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

// AVI
void Logger::aviInit(int width, int height) {
    _aviFile = _sd.open("/video.avi", FILE_WRITE);
    _avi.begin(_aviFile, width, height);
}

void Logger::aviStart() {
    _avi.startRecording();
}

// AVI動画撮影
void Logger::aviRecord(void* buff, size_t size) {
    _avi.addFrame(buff, size);
}

// AVI撮影終了
void Logger::aviEnd() {
    _avi.endRecording();
    _avi.end();
}