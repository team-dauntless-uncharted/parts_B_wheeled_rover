#include "BaseFileIO.hpp"

BaseFileIO::BaseFileIO() {}

// --- POSIX API ---

bool BaseFileIO::posixOpen(const char* path, bool write, int &fd, bool truncate = false) {
	int flags = 0;

	if (write) {
		flags = O_WRONLY | O_CREAT;
		if (truncate) {
			flags |= O_TRUNC;
		} else {
			flags |= O_APPEND;
		}
	} else {
		flags = O_RDONLY;
	}

	fd = ::open(path, flags, 0666);
	return fd >= 0;
}

void BaseFileIO::posixClose(int &fd) {
	if (fd >= 0) {
		::close(fd);
		fd = -1;
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

	char buf[32] = {};
	ssize_t n = posixRead(fd, buf, sizeof(buf) - 1);
	posixClose(fd);

	if (n <= 0) return false;
	state = atoi(buf);
	return true;
}

bool BaseFileIO::writeState(const char *path, const int &state) {
	int fd;
	if (!posixOpen(path, true, fd, true)) return false;

	char buf[16];
	int n = snprintf(buf, sizeof(buf), "%d", state);
	posixWrite(fd, buf, n);
	posixClose(fd);
	return true;
}
