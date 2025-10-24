/**
 * @file FlashIO.hpp
 * @brief Flash ストレージへのファイルI/O操作
 */

#pragma once

#ifdef USE_FLASH

#include <Arduino.h>
#include "Utils/FileIO/BaseFileIO.hpp"

/**
 * @class FlashIO
 * @brief Spresense 内蔵 Flash ストレージにファイルを読み書きするクラス
 *
 * BaseFileIO を継承し、Flash ストレージ（/mnt/spif）へのアクセスを提供する
 * 主に状態番号（state.txt）の永続化に使用する
 * SDカードへの書き込みができなくなったときに動ける状態に復帰するために使用する
 *
 * 注意事項:
 * - Flash メモリは書き込み回数に制限がある（数万〜数十万回）
 * - コンパイル時に USE_FLASH フラグが必要
 *
 * 使用方法:
 * @code
 * #ifdef USE_FLASH
 * FlashIO flash;
 * flash.begin();
 * int state = 0;
 * flash.readState(state);
 * @endcode
 */
class FlashIO : public BaseFileIO {
public:
	/**
	 * @brief コンストラクタ
	 */
	FlashIO();

	/**
	 * @brief Flash ストレージを初期化する
	 * @return true: 成功, false: 失敗
	 *
	 * Flash ストレージのマウント処理を行う
	 */
	bool begin();

	/**
	 * @brief 状態番号を Flash から読み取る
	 * @param state 読み取った状態番号を格納する変数への参照
	 * @return true: 成功, false: 失敗
	 */
	bool readState(int &state);

	/**
	 * @brief 状態番号を Flash に書き込む
	 * @param state 書き込む状態番号
	 * @return true: 成功, false: 失敗
	 */
	bool writeState(const int &state);

protected:
	/**
	 * @brief Flash ストレージのベースパスを取得する
	 * @return "/mnt/spif"（Spresense Flash マウントポイント）
	 */
	const char *getBasePath() const override;
};

#endif // USE_FLASH