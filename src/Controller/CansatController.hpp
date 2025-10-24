/**
 * @file CansatController.hpp
 * @brief CanSat全体を統括する中央コントローラー
 */

#pragma once
#include "Sensor/Gnss/GnssSensor.hpp"
#include "Sensor/CdS/CdSSensor.hpp"
#include "Sensor/Camera/CameraController.hpp"
#include <BNO055Library.h>

#include "Actuator/Motor/Motor.hpp"
#include "Actuator/Led/Led.hpp"
#include "Actuator/Speaker/Speaker.hpp"

#include "Utils/FileIO/SDLogger.hpp"
#ifdef USE_FLASH
#include "Utils/FileIO/FlashIO.hpp"
#endif // USE_FLASH
#include "Utils/PowerController/PowerController.hpp"
#include "Utils/Serial/SerialWriter.hpp"
#include <TwelitePacket.h>

#include "Controller/ICansatState.hpp"
#include <array>
#include <cstdarg>
#include <cstdio>

/**
 * @def SENSOR_BUFFER_SIZE
 * @brief センサーログのバッファサイズ（バイト）
 *
 * センサーデータをメモリ上にバッファリングし、
 * まとめてSDカードに書き込むことで効率化する
 */
#define SENSOR_BUFFER_SIZE 4096

/**
 * @struct UserConfig
 * @brief ユーザー設定可能なパラメータ
 *
 * config.json ファイルから読み込まれる設定値を保持する。
 * ファイルが存在しない、または特定のキーがない場合は、
 * ここで定義されたデフォルト値が使用される
 */
struct UserConfig {
    unsigned long calibrationStateTimeoutThreshold = 5 * 60 * 1000;  ///< CALIBRATIONタイムアウト（ミリ秒）
    double standbyStateAltThreshold = 20;                            ///< STANDBYでの高度閾値
    unsigned long standbyStateTimeoutThreshold = 10 * 60 * 1000;     ///< STANDBYタイムアウト（ミリ秒）
    int launchStateCdsThreshold = 400;                               ///< LAUNCHのCdS閾値
    unsigned long launchStateTimeoutThreshold = 20 * 60 * 1000;      ///< LAUNCHタイムアウト（ミリ秒）
    unsigned long dropStateTimeoutThreshold = 10 * 60 * 1000;        ///< DROPタイムアウト（ミリ秒）
    unsigned long escapeStateTimeoutThreshold = 15 * 1000;           ///< ESCAPEタイムアウト（ミリ秒）
    int detectionMaxFailedCount = 25;                                ///< DETECTION失敗上限回数
    int detectionTurn180delay = 700;                                 ///< DETECTION 180度回転時間（ミリ秒）
    unsigned long recordingTimeoutThreshold = 5 * 60 * 1000;         ///< RECORDINGタイムアウト（ミリ秒）
    int recordingTime = 30 * 1000;                                   ///< RECORDING録画時間（ミリ秒）
};

/**
 * @class CansatController
 * @brief CanSat全体を統括する中央コントローラークラス
 *
 * すべてのセンサー、アクチュエータ、ログ機能を管理し、
 * ステートマシンパターンで動作する
 *
 * 主な機能:
 * - センサーデータの一括管理（GNSS, IMU, CdS, Camera）
 * - アクチュエータ制御（Motor, LED, Speaker）
 * - 状態遷移管理（State Pattern）
 * - ログ管理（SD/Flash）
 * - 設定ファイル読み込み（config.json）
 */
class CansatController {
public:
    /**
     * @brief コンストラクタ
     */
    CansatController();

    /**
     * @brief システム全体を初期化する
     */
    void begin();

    /**
     * @brief メインループで周期的に呼び出す更新処理
     *
     * 約200msごとに呼び出される
     * 以下の処理を実行する
     * 1. ハートビートログ（10秒ごと）
     * 2. 全センサー値の更新
     * 3. センサーログのバッファリング
     * 4. 現在の状態の onUpdate() 呼び出し
     */
    void update();

    /**
     * @brief 状態を変更する
     * @param newState 新しい状態のユニークポインタ
     *
     * 状態遷移の流れ:
     * 1. 現在の状態の onExit() を呼び出し
     * 2. 状態ポインタを移動
     * 3. 新しい状態の onEnter() を呼び出し
     */
    void changeState(std::unique_ptr<ICansatState> newState);

    /**
     * @brief システムログを出力する（printf形式）
     * @param format フォーマット文字列
     * @param ... 可変長引数
     */
    void writeSystemLog(const char* format, ...);

    // --- センサーアクセス ---

    GnssSensor &getGnss() { return _gnss; }                ///< GNSS センサーへの参照取得
    BNO055 &getBno055() { return _bno055; }                ///< IMU センサーへの参照取得
    CdSSensor &getCds() { return _cds; }                   ///< CdS センサーへの参照取得
    CameraController &getCamera() { return _camera; }      ///< カメラへの参照取得
    PowerController &getPower() { return _power; }         ///< 電源管理への参照取得

    // --- アクチュエータアクセス ---

    Motor &getMotor() { return _motor; }                   ///< モーターへの参照取得
    Led &getLed(int idx) { return _led[idx]; }             ///< LED（0-3）への参照取得
    Speaker &getSpeaker() { return _speaker; }             ///< スピーカーへの参照取得

    // --- データ管理アクセス ---

    twelite::TwelitePacket &getTwelite() { return _twelite; }  ///< Twelite無線通信への参照取得
    SDLogger &getSDLogger() { return _sdLogger; }          ///< SD ロガーへの参照取得
#ifdef USE_FLASH
    FlashIO &getFlashIO() { return _flash; }               ///< Flash I/Oへの参照取得
#endif // USE_FLASH
    SerialWriter &getSerialWriter() { return _writer; }    ///< シリアルライターへの参照取得

    /**
     * @brief Twelite接続フラグを設定する
     * @param isConnectTwelite 接続済みかどうか
     */
    void setIsConnectTwelite(bool isConnectTwelite) { _isConnectTwelite = isConnectTwelite; }

    /**
     * @brief カメラ初期化フラグを設定する
     * @param initCamera 初期化済みかどうか
     */
    void setInitCamera(bool initCamera) { _initCamera = initCamera; }

    /**
     * @brief Tweliteが接続済みかを確認する
     * @return true: 接続済み, false: 未接続
     */
    bool isConnectTwelite() { return _isConnectTwelite; }

    /**
     * @brief カメラが初期化済みかを確認する
     * @return true: 初期化済み, false: 未初期化
     */
    bool isInitCamera() { return _initCamera; }

    /**
     * @brief ユーザー設定を取得する
     * @return UserConfig 構造体のコピー
     */
    UserConfig getUserConfig() { return _config; }

    /**
     * @brief センサーログをバッファに追記する
     *
     * 現在のセンサー値をCSV形式でバッファに追記する
     * バッファが満杯になったら自動的にSDカードに書き込む
     */
    void appendSensorLog();

    /**
     * @brief LEDをビットフラグで制御する
     * @param state ビットフラグ（bit0=LED0, bit1=LED1, bit2=LED2, bit3=LED3）
     *
     * 状態番号をビットパターンとしてLEDに表示する
     * 例: state=5 (0b0101) → LED0とLED2が点灯
     */
    void setLed(const int state);

private:
    UserConfig _config;                 ///< ユーザー設定（config.jsonから読み込み）
    unsigned long _lastHeartBeat = 0;   ///< 最後のハートビート出力時刻（ミリ秒）

    /**
     * @brief 現在の設定をログに出力する
     *
     * config.jsonから読み込んだ設定値をシステムログに出力する
     */
    void dumpConfig();

    /**
     * @brief config.jsonファイルを読み込む
     *
     * SDカードの /mnt/sd0/config.json を読み込み、
     * UserConfig 構造体の各フィールドを更新する
     * ファイルが存在しないかパースに失敗した場合は何もしません。
     */
    void readConfigFile();

    /**
     * @brief 初期状態を設定する
     *
     * SDカード（またはFlash）から state.txt を読み取り、
     * 対応する状態のインスタンスを生成して changeState() を呼び出す
     * ファイルが存在しない場合は CalibrationState から開始する
     */
    void configState();

    // --- センサーログバッファ ---
    const String CSV_HEADER = "time,date,mode,lat,lng,alt,cds,ax,ay,az,gx,gy,gz,mx,my,mz,roll,pitch,heading,voltage"; ///< CSVヘッダー
    char _sensorBuffer[SENSOR_BUFFER_SIZE];  ///< センサーデータバッファ
    size_t _head = 0;                        ///< バッファの書き込み位置

    // --- 状態管理 ---
    std::unique_ptr<ICansatState> _state;  ///< 現在の状態（State Pattern）

    bool _isConnectTwelite = false;  ///< Twelite接続済みフラグ
    bool _initCamera = false;  ///< カメラ初期化済みフラグ

    // --- センサー ---
    GnssSensor _gnss;             ///< GNSS センサー
    BNO055 _bno055;               ///< 9軸 IMU センサー
    Vector<float> _acceleration;  ///< 加速度ベクトル（BNO055から取得）
    Vector<float> _gyro;          ///< ジャイロベクトル（BNO055から取得）
    Vector<float> _magnetic;      ///< 磁気ベクトル（BNO055から取得）
    EulerAngles _euler;           ///< オイラー角（roll, pitch, heading）
    CdSSensor _cds;               ///< CdS 光センサー
    CameraController _camera;     ///< カメラコントローラー

    // --- アクチュエータ ---
    int _motorR_pins[3];        ///< 右モーターピン配列
    int _motorL_pins[3];        ///< 左モーターピン配列
    Motor _motor;               ///< モーターコントローラー
    std::array<Led, 4> _led;    ///< LED配列（LED0-LED3）
    Speaker _speaker;           ///< スピーカー

    PowerController _power;     ///< 電源管理

    // --- データ管理 ---
    twelite::TwelitePacket _twelite;  ///< Twelite無線通信
    SDLogger _sdLogger;         ///< SDカードロガー
#ifdef USE_FLASH
    FlashIO _flash;             ///< Flash ストレージI/O
#endif // USE_FLASH
    SerialWriter _writer;       ///< シリアル出力
};