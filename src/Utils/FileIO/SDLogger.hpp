/**
 * @file SDLogger.hpp
 * @brief SDカードへのログ・画像・動画保存を行うクラス
 */

#pragma once
#include <Arduino.h>
#include <PosixAvi.hpp>
#include "Utils/FileIO/BaseFileIO.hpp"

/**
 * @class SDLogger
 * @brief SDカードにログ、画像、動画を保存するクラス
 *
 * BaseFileIO を継承し、SDカード（/mnt/sd0）へのアクセスを提供する
 * - システムログ（system_log_XXXX.txt）
 * - センサーログ（sensor_log_XXXX.csv）
 * - JPEG画像（jpeg_XXXX.jpg）
 * - PPM画像（ppm_XXXX.ppm）
 * - AVI動画（avi_XXXX.avi）
 * - 状態ファイル（state.txt）
 * - JSON設定ファイル（config.json）
 *
 * ファイル名は自動で連番管理され、既存ファイルがあれば次の番号が使用される
 */
class SDLogger : public BaseFileIO {
public:
    /**
     * @brief コンストラクタ
     */
    SDLogger();

    /**
     * @brief SDカードを初期化し、ログファイルを準備する
     * @param csvHeader センサーログのCSVヘッダー行
     * @return true: 成功, false: 失敗
     */
    bool begin(const String &csvHeader);

    /**
     * @brief システムログを追記する
     * @param message ログメッセージ
     * @return true: 成功, false: 失敗
     */
    bool appendSystemLog(const char* message);

    /**
     * @brief センサーログを追記する
     * @param message CSVフォーマットのログメッセージ
     * @return true: 成功, false: 失敗
     */
    bool appendSensorLog(const char* message);

    /**
     * @brief センサーログを追記する（サイズ指定版）
     * @param message CSVフォーマットのログメッセージ
     * @param length メッセージのバイト数
     * @return true: 成功, false: 失敗
     */
    bool appendSensorLog(const char* message, size_t length);

    /**
     * @brief JPEG画像をSDカードに保存する
     * @param buff JPEG画像データのバッファ
     * @param size 画像データのサイズ（バイト）
     * @return true: 成功, false: 失敗
     */
    bool saveJPEGImage(void* buff, size_t size);

    /**
     * @brief PPM画像をSDカードに保存する
     * @param buff PPM画像データのバッファ
     * @param size 画像データのサイズ（バイト）
     * @return true: 成功, false: 失敗
     */
    bool savePPMImage(void* buff, size_t size);

    // --- AVI動画録画関連 ---

    /**
     * @brief AVI録画を初期化する
     * @param width 動画の幅（ピクセル）
     * @param height 動画の高さ（ピクセル）
     * @return true: 成功, false: 失敗
     */
    bool aviInit(int width, int height);

    /**
     * @brief AVI録画を開始する
     * @return true: 成功, false: 失敗
     */
    bool aviStart();

    /**
     * @brief AVIフレームを記録する
     * @param buff フレームデータのバッファ
     * @param size フレームデータのサイズ（バイト）
     * @return true: 成功, false: 失敗
     */
    bool aviRecord(void* buff, size_t size);

    /**
     * @brief AVI録画を終了する
     * @return true: 成功, false: 失敗
     */
    bool aviEnd();

    /**
     * @brief AVIエラー状態を確認する
     * @return true: エラーあり, false: エラーなし
     */
    bool aviHasFailed() const { return _avi.hasFailed(); }

    /**
     * @brief AVIエラーメッセージを取得する
     * @return エラーメッセージ文字列
     */
    const char* aviGetErrorMessage() const { return _avi.getErrorMessage(); }

    /**
     * @brief AVI録画を緊急停止する
     */
    void aviEmergencyStop();

    /**
     * @brief 状態番号をSDカードから読み取る
     * @param state 読み取った状態番号を格納する変数への参照
     * @return true: 成功, false: 失敗
     */
    bool readState(int &state);

    /**
     * @brief 状態番号をSDカードに書き込む
     * @param state 書き込む状態番号
     * @return true: 成功, false: 失敗
     */
    bool writeState(const int &state);

    /**
     * @brief JSONファイルを読み込む
     * @param filename ファイル名
     * @param buffer 読み込んだデータを格納するバッファ
     * @param bufferSize バッファサイズ（バイト）
     * @return 読み取ったバイト数（エラー時は0）
     */
    size_t readJSONFile(const char* filename, char* buffer, size_t bufferSize);

protected:
    /**
     * @brief SDカードのベースパスを取得する
     * @return "/mnt/sd0"（Spresense SDカードマウントポイント）
     */
    const char *getBasePath() const override;

private:
    /**
     * @brief SDカードのマウントを待機する
     * @param timeout_ms タイムアウト時間（ミリ秒、デフォルト5000ms）
     * @return true: マウント成功, false: タイムアウト
     */
    bool waitForSDMount(int timeout_ms = 5000);

    /**
     * @brief ファイル名の連番インデックスを更新する（汎用）
     * @param fileNameBuf ファイル名バッファ
     * @param bufSize バッファサイズ
     * @param format ファイル名フォーマット（例: "/mnt/sd0/log_%04d.txt"）
     * @param counter カウンター変数への参照
     */
    void refreshFileNameIndex(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter);

    /**
     * @brief ファイル名をシフトする（汎用）
     * @param fileNameBuf ファイル名バッファ
     * @param bufSize バッファサイズ
     * @param format ファイル名フォーマット
     * @param counter カウンター変数への参照
     */
    void shiftFileName(char* fileNameBuf, size_t bufSize, const char* format, uint16_t& counter);

    // --- SystemLog ---
    char _systemLogFileName[32];          ///< システムログファイル名
    uint16_t _systemLogFileNameCount = 0; ///< システムログファイル連番

    void refreshSystemLogFileNameIndex();
    void shiftSystemLogFileName();

    // --- SensorLog ---
    char _sensorLogFileName[32];          ///< センサーログファイル名
    uint16_t _sensorLogFileNameCount = 0; ///< センサーログファイル連番

    void refreshSensorLogFileNameIndex();
    void shiftSensorLogFileName();

    /**
     * @brief ログファイルにメッセージを追記する（内部用）
     * @param filename ファイル名
     * @param message メッセージ
     * @return true: 成功, false: 失敗
     */
    bool appendLog(const char* filename, const char* message);

    // --- JPEG ---
    char _jpegFileName[32];          ///< JPEGファイル名
    uint16_t _jpegFileNameCount = 0; ///< JPEGファイル連番

    void refreshJPEGFileNameIndex();
    void shiftJPEGFileName();

    // --- PPM ---
    char _ppmFileName[32];          ///< PPMファイル名
    uint16_t _ppmFileNameCount = 0; ///< PPMファイル連番

    void refreshPPMFileNameIndex();
    void shiftPPMFileName();

    // --- AVI ---
    char _aviFileName[32];                ///< AVIファイル名
    uint16_t _aviFileNameCount = 0;       ///< AVIファイル連番
    bool _aviRecordingActive = false;     ///< 録画状態の追跡フラグ

    void refreshAVIFileNameIndex();
    void shiftAVIFileName();

    PosixAviLibrary _avi;  ///< PosixAvi ライブラリインスタンス
};