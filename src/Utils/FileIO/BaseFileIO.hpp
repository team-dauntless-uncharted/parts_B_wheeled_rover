/**
 * @file BaseFileIO.hpp
 * @brief ファイルI/O操作の基底クラス
 */

#pragma once
#include <Arduino.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>

/**
 * @class BaseFileIO
 * @brief ファイルI/O操作の共通機能を提供する抽象基底クラス
 *
 * POSIX API（open, read, write, close）を使用したファイル操作と、
 * 状態管理（state.txt）の読み書き機能を提供する
 * SDLogger と FlashIO の共通基底クラス
 */
class BaseFileIO {
public:
	/**
	 * @brief コンストラクタ
	 */
	BaseFileIO();

	/**
	 * @brief 仮想デストラクタ
	 */
	virtual ~BaseFileIO() = default;

	/**
	 * @brief 状態番号をファイルから読み取る
	 * @param path ファイルパス
	 * @param state 読み取った状態番号を格納する変数への参照
	 * @return true: 成功, false: 失敗
	 *
	 * テキストファイルから整数値を読み取り、状態番号として取得する
	 * state.txt ファイルから現在の状態を復元するために使用する
	 * 途中で再起動が入ってしまった際などの復帰用
	 */
	bool readState(const char *path, int &state);

	/**
	 * @brief 状態番号をファイルに書き込む
	 * @param statePath ファイルパス
	 * @param state 書き込む状態番号
	 * @return true: 成功, false: 失敗
	 */
	bool writeState(const char *statePath, const int &state);

protected:
	/**
	 * @brief POSIXファイルを開く
	 * @param path ファイルパス
	 * @param write true: 書き込みモード, false: 読み取りモード
	 * @param fd ファイルディスクリプタを格納する変数への参照
	 * @param truncate true: ファイルを切り詰め（上書き）, false: 追記
	 * @return true: 成功, false: 失敗
	 */
	bool posixOpen(const char* path, bool write, int &fd, bool truncate = false);

	/**
	 * @brief POSIXファイルを閉じる
	 * @param fd ファイルディスクリプタへの参照
	 */
	void posixClose(int &fd);

	/**
	 * @brief POSIXファイルにデータを書き込む
	 * @param fd ファイルディスクリプタ
	 * @param buf 書き込むデータのバッファ
	 * @param size 書き込むバイト数
	 * @return 実際に書き込まれたバイト数（エラー時は負の値）
	 */
	ssize_t posixWrite(int fd, const void *buf, size_t size);

	/**
	 * @brief POSIXファイルからデータを読み取る
	 * @param fd ファイルディスクリプタ
	 * @param buf 読み取ったデータを格納するバッファ
	 * @param size 読み取るバイト数
	 * @return 実際に読み取ったバイト数（エラー時は負の値）
	 */
	ssize_t posixRead(int fd, void *buf, size_t size);

	/**
	 * @brief ベースパスを取得する（純粋仮想関数）
	 * @return ベースパス文字列（例: "/mnt/sd0", "/mnt/spif"）
	 */
	virtual const char *getBasePath() const = 0;
};