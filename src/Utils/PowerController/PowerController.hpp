/**
 * @file PowerController.hpp
 * @brief 電源管理とブート原因取得を行うクラス
 */

#pragma once

#include <LowPower.h>

/**
 * @class PowerController
 * @brief Spresense の電源管理機能を制御するクラス
 *
 * LowPower ライブラリをラップし、以下の機能を提供する
 * - バッテリー電圧の取得
 * - ブート原因の判定（通常起動、リセット、スリープからの復帰など）
 */
class PowerController {
public:
	/**
	 * @brief コンストラクタ
	 */
	PowerController();

	/**
	 * @brief 電源管理機能を初期化する
	 */
	void begin();

	/**
	 * @brief バッテリー電圧を取得する
	 * @return バッテリー電圧（ミリボルト単位）
	 */
	int getVoltage();

	/**
	 * @brief ブート原因を取得する
	 * @return ブート原因を示す文字列
	 *
	 * システムが起動した理由を判定する
	 * - 通常の電源投入（Power On）
	 * - Watchdog による再起動/リセット
	 * - DeepSleep からの復帰（WKUPL/WKUPS/RTC/USB）
	 * - ColdSleep からの復帰（RTC/GPIO/USB/センサ割り込み）
	 */
	const char *getBootCause();
};