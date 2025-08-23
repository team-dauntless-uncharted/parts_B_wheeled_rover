#pragma once
#include <Arduino.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>

class BaseFileIO {
public:
	BaseFileIO();
	virtual ~BaseFileIO() = default;

	// State管理(サブクラスで共通して使用)
	bool readState(const char *path, int &state);
	bool writeState(const char *statePath, const int &state);

protected:
	// POSIXファイル操作
	bool posixOpen(const char* path, bool write, int &fd, bool truncate = false);
	void posixClose(int &fd);
	ssize_t posixWrite(int fd, const void *buf, size_t size);
	ssize_t posixRead(int fd, void *buf, size_t size);

	virtual const char *getBasePath() const = 0;
};