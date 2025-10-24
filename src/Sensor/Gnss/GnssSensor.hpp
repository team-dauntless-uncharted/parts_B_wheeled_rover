/**
 * @file GnssSensor.hpp
 * @brief 内蔵GNSS（GPS/準天頂衛星）の制御
 */

#pragma once

#include <GNSS.h>
#include <functional>

/**
 * @class GnssSensor
 * @brief Spresense内蔵GNSSモジュールを制御するクラス
 *
 * - GPS/準天頂衛星(QZ_L1CA, QZ_L1S)からの測位
 * - 非ブロッキング更新（update()）とブロッキング待機（waitReceive()）
 * - 割り込みベースのデータ受信通知（オプション）
 */
class GnssSensor {
public:
	/**
	 * @brief コンストラクタ
	 * @param timeout_ms データ更新のタイムアウト時間（ミリ秒）
	 *
	 * GNSSセンサーを初期化し、各変数を初期値に設定する
	 */
	GnssSensor(int timeout_ms);

	/**
	 * @brief デストラクタ
	 *
	 * 割り込みモードを無効化し、リソースを解放する
	 */
	~GnssSensor();

	/**
	 * @brief GNSSモジュールを初期化して測位を開始する
	 * @return true: 成功, false: 失敗
	 *
	 * GPS衛星と準天頂衛星（QZ_L1CA, QZ_L1S）を選択し、
	 * COLD_STARTモードで測位を開始する
	 */
	bool begin();

	/**
	 * @brief GNSSデータを更新する（非ブロッキング）
	 * @return true: データ更新あり, false: データ更新なし
	 *
	 * waitUpdate(0)で即座に戻り、データが更新されていればtrue、
	 * なければfalseを返します。メインループで定期的に呼び出す
	 */
	bool update();

	/**
	 * @brief 現在の緯度を取得する
	 * @return 緯度（度、10進数形式）
	 */
	double getLatitude() const;

	/**
	 * @brief 現在の経度を取得する
	 * @return 経度（度、10進数形式）
	 */
	double getLongitude() const;

	/**
	 * @brief 現在の高度を取得する
	 * @return 高度（メートル）
	 */
	double getAltitude() const;

	/**
	 * @brief 現在の日時を取得する
	 * @return 日時文字列（"YYYY/MM/DD HH:MM:SSZ" 形式）へのポインタ
	 */
	char *getCurrentDate() const;

	/**
	 * @brief 測位が完了しているかを確認する
	 * @return true: 測位完了, false: 測位未完了
	 */
	bool isPosFix() const;

	/**
	 * @brief 測位完了まで待機する（ブロッキング）
	 * @return true: 測位成功, false: タイムアウト（5分）
	 *
	 * isPosFix()がtrueになるまで最大5分間待機する
	 * コールドスタート時に使用する
	 * WAIT_GNSS_RECEIVEフラグでコンパイル時に有効化する
	 */
	bool waitReceive();
	
	// --- 割り込みベースの非同期処理（オプション機能） ---

	/**
	 * @brief 割り込みモードを有効化する
	 *
	 * GNSSデータ受信時にコールバックを実行する非同期モードを有効にする
	 */
	void enableInterruptMode();

	/**
	 * @brief 割り込みモードを無効化する
	 */
	void disableInterruptMode();

	/**
	 * @brief データ受信時のコールバック関数を設定する
	 * @param callback データ受信時に呼び出される関数
	 *
	 * 割り込みモードでGNSSデータが更新されたときに実行される
	 */
	void setDataReadyCallback(std::function<void()> callback);

	/**
	 * @brief データ受信フラグを確認する
	 * @return true: 新しいデータあり, false: なし
	 */
	bool isDataReady() const;

	/**
	 * @brief データ受信フラグをクリアする
	 */
	void clearDataReadyFlag();

	// --- タイマー割り込みベースの処理（オプション機能） ---

	/**
	 * @brief タイマー割り込みを有効化する
	 * @param interval_ms 割り込み間隔（ミリ秒、デフォルト1000ms）
	 *
	 * 定期的にGNSSデータを自動更新するタイマー割り込みを設定する
	 */
	void enableTimerInterrupt(unsigned long interval_ms = 1000);

	/**
	 * @brief タイマー割り込みを無効化する
	 */
	void disableTimerInterrupt();

	/**
	 * @brief タイマー割り込みハンドラー
	 *
	 * タイマー割り込み時に呼び出され、update()を実行する
	 */
	void handleTimerInterrupt();

private:
	SpGnss _gnss;           ///< Spresense GNSS オブジェクト
	double _latitude;       ///< 緯度（度）
	double _longitude;      ///< 経度（度）
	double _altitude;       ///< 高度（メートル）
	bool _posFix;           ///< 測位完了フラグ
	static const int STRING_BUFFER_SIZE = 128;
	char _currentDate[STRING_BUFFER_SIZE];  ///< 日時文字列バッファ

	int _timeout_ms = 1000;  ///< データ更新タイムアウト（ミリ秒）

	// 割り込み処理用の変数
	volatile bool _dataReady;              ///< データ受信完了フラグ
	volatile bool _interruptModeEnabled;   ///< 割り込みモード有効フラグ
	volatile bool _timerInterruptEnabled;  ///< タイマー割り込み有効フラグ
	volatile unsigned long _lastUpdateTime;  ///< 最終更新時刻
	std::function<void()> _dataReadyCallback;  ///< データ受信時コールバック関数

	// 内部メソッド
	/**
	 * @brief GNSSデータ受信時に呼ばれる内部処理
	 *
	 * _dataReadyフラグを立て、コールバック関数があれば実行する
	 */
	void onGnssDataReceived();

	/**
	 * @brief 静的割り込みハンドラー
	 *
	 * C言語スタイルの割り込みハンドラーから呼び出される
	 */
	static void gnssInterruptHandler();

	static GnssSensor* _instance;  ///< 静的インスタンスポインタ（割り込みハンドラー用）
};