#pragma once

#include <GNSS.h>
#include <functional>

class GnssSensor {
public:
	GnssSensor(int timeout_ms);
	~GnssSensor();
	
	bool begin();
	bool update();
	double getLatitude() const;
	double getLongitude() const;
	double getAltitude() const;
	char *getCurrentDate() const;
	bool isPosFix() const;
	// データが受信できるまで待つ
	bool waitReceive();
	
	// 割り込みベースの非同期処理
	void enableInterruptMode();
	void disableInterruptMode();
	void setDataReadyCallback(std::function<void()> callback);
	bool isDataReady() const;
	void clearDataReadyFlag();
	
	// タイマー割り込みベースの処理
	void enableTimerInterrupt(unsigned long interval_ms = 1000);
	void disableTimerInterrupt();
	void handleTimerInterrupt();

private:
	SpGnss _gnss;
	double _latitude;
	double _longitude;
	double _altitude;
	bool _posFix;
	static const int STRING_BUFFER_SIZE = 128;
	char _currentDate[STRING_BUFFER_SIZE];

	int _timeout_ms = 1000;
	
	// 割り込み処理用の変数
	volatile bool _dataReady;
	volatile bool _interruptModeEnabled;
	volatile bool _timerInterruptEnabled;
	volatile unsigned long _lastUpdateTime;
	std::function<void()> _dataReadyCallback;
	
	// 内部メソッド
	void onGnssDataReceived();
	static void gnssInterruptHandler();
	static GnssSensor* _instance; // 静的インスタンスポインタ（割り込みハンドラー用）
};