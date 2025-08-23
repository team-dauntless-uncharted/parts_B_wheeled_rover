#include "FlashIO.hpp"

#ifdef USE_FLASH

FlashIO::FlashIO() : BaseFileIO() {}

const char *FlashIO::getBasePath() const {
	return "/mnt/spif";
}

bool FlashIO::begin() {
	return true;
}

bool FlashIO::readState(int &state) {
	return BaseFileIO::readState("/mnt/spif/state.txt", state);
}

bool FlashIO::writeState(const int &state) {
	return BaseFileIO::writeState("/mnt/spif/state.txt", state);
}

#endif // USE_FLASH