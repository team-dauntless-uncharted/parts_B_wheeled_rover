#include "Utils/Logger/Logger.hpp"
#include <Arduino.h>

Logger::Logger() : _sd() {}

// 初期化
bool Logger::begin(String csvHeader) {
    if (!sdInit()) {
        return false;
    }
    
    refreshSystemLogFileNameIndex();
    refreshSensorLogFileNameIndex();
    refreshJPEGFileNameIndex();
    refreshPPMFileNameIndex();

    appendSensorLog(csvHeader.c_str());
    
    return true;
}

// ログの追加
bool Logger::appendLog(const char* filename, const char* message) {
    File logFile = _sd.open(filename, FILE_WRITE);
    
    if (!logFile) {
        return false;
    }
    
    logFile.println(message);
    logFile.close();
    
    return true;
}

bool Logger::appendSystemLog(const char* message) {
    return appendLog(_systemLogFileName, message);
}

bool Logger::appendSensorLog(const char* message) {
    return appendLog(_sensorLogFileName, message);
}

bool Logger::sdInit() {
    if (!_sd.begin()) {
        return false;
    }
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

void Logger::refreshSystemLogFileNameIndex() {
    refreshFileNameIndex(_systemLogFileName, sizeof(_systemLogFileName), "/system_%04d.log", _systemLogFileNameCount);
}

void Logger::shiftSystemLogFileName() {
    shiftFileName(_systemLogFileName, sizeof(_systemLogFileName), "/system_%04d.log", _systemLogFileNameCount);
}

void Logger::refreshSensorLogFileNameIndex() {
    refreshFileNameIndex(_sensorLogFileName, sizeof(_sensorLogFileName), "/sensor_%04d.csv", _sensorLogFileNameCount);
}

void Logger::shiftSensorLogFileName() {
    shiftFileName(_sensorLogFileName, sizeof(_sensorLogFileName), "/sensor_%04d.csv", _sensorLogFileNameCount);
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

void Logger::refreshAVIFileNameIndex() {
    refreshFileNameIndex(_aviFileName, sizeof(_aviFileName), "/video_%04d.avi", _aviFileNameCount);
}

void Logger::shiftAVIFileName() {
    shiftFileName(_aviFileName, sizeof(_aviFileName), "/video_%04d.avi", _aviFileNameCount);
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

// PPMファイルの保存
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
    refreshAVIFileNameIndex();
    _aviFile = _sd.open(_aviFileName, FILE_WRITE);
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

bool Logger::readState(int &state) {
    File stateFile = _sd.open("/state.txt", FILE_READ);
    if (!stateFile) {
        return false;
    }
    
    String stateStr = stateFile.readString();
    stateFile.close();
    
    state = stateStr.toInt();
    return true;
}

bool Logger::writeState(const int &state) {
    // ファイルを読み書き両用で開く
    File stateFile = _sd.open("/state.txt", FILE_WRITE);
    if (!stateFile) {
        return false;
    }
    
    // ファイルポジションを先頭に設定
    stateFile.seek(0);
    
    // ファイルにintを書き込み
    if (stateFile.write((uint8_t*)&state, sizeof(state)) != sizeof(state)) {
        stateFile.close();
        return false;
    }
    
    // ファイルサイズを現在のポジションに切り詰め
    stateFile.flush();
    stateFile.close();
    return true;
}