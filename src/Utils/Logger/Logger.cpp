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

// ファイル名のインデックスを開始位置に設定
void Logger::refreshFileNameIndex(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter) {
    while (true) {
        shiftFileName(fileNameBuf, bufSize, format, counter);
        if (!_sd.exists(fileNameBuf)) {
            break;
        }
    }
}

// ファイル名をインクリメントして更新
void Logger::shiftFileName(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter) {
    snprintf(fileNameBuf, bufSize, format, counter);
    counter++;
}

void Logger::refreshJPEGFileNameIndex() {
    refreshFileNameIndex(_jpegFileName, sizeof(_jpegFileName), "/explore_%04d.jpg", _jpegFileNameCount);
}

void Logger::shiftJPEGFileName() {
    shiftFileName(_jpegFileName, sizeof(_jpegFileName), "/explore_%04d.jpg", _jpegFileNameCount);
}

void Logger::refreshPPMFileNameIndex() {
    refreshFileNameIndex(_ppmFileName, sizeof(_ppmFileName), "/detection_%04d.ppm", _ppmFileNameCount);
}

void Logger::shiftPPMFileName() {
    shiftFileName(_ppmFileName, sizeof(_ppmFileName), "/detection_%04d.ppm", _ppmFileNameCount);
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