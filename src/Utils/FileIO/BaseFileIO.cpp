/**
 * @file BaseFileIO.cpp
 * @brief ファイルI/O基底クラスの実装
 */

#include "BaseFileIO.hpp"

BaseFileIO::BaseFileIO() {}

// --- POSIX API ラッパー ---

bool BaseFileIO::posixOpen(const char* path, bool write, int &fd, bool truncate) {
	int flags = 0;

	if (write) {
		// 書き込みモード: 作成 + 書き込み専用
		flags = O_WRONLY | O_CREAT;
		if (truncate) {
			flags |= O_TRUNC;   // 既存ファイルを切り詰め（上書き）
		} else {
			flags |= O_APPEND;  // 追記モード
		}
	} else {
		// 読み取りモード
		flags = O_RDONLY;
	}

	// ファイルを開く（パーミッション: 0666 = rw-rw-rw-）
	fd = ::open(path, flags, 0666);
	return fd >= 0;
}

void BaseFileIO::posixClose(int &fd) {
	if (fd >= 0) {
		::close(fd);
		fd = -1;  // 無効なディスクリプタとしてマーク
	}
}

ssize_t BaseFileIO::posixWrite(int fd, const void* buf, size_t size) {
	return ::write(fd, buf, size);
}

ssize_t	BaseFileIO::posixRead(int fd, void* buf, size_t size) {
	return ::read(fd, buf, size);
}

// --- State管理 ---

bool BaseFileIO::readState(const char *path, int &state) {
	int fd;
	if (!posixOpen(path, false, fd)) return false;

	// ファイル内容を読み取る（最大31文字 + null終端）
	char buf[32] = {};
	ssize_t n = posixRead(fd, buf, sizeof(buf) - 1);
	posixClose(fd);

	if (n <= 0) return false;

	// 文字列を整数に変換
	state = atoi(buf);
	return true;
}

bool BaseFileIO::writeState(const char *path, const int &state) {
	int fd;
	// truncate=true で既存ファイルを上書き
	if (!posixOpen(path, true, fd, true)) return false;

	// 状態番号を文字列化
	char buf[16];
	int n = snprintf(buf, sizeof(buf), "%d", state);

	// ファイルに書き込み
	posixWrite(fd, buf, n);
	posixClose(fd);
	return true;
}
