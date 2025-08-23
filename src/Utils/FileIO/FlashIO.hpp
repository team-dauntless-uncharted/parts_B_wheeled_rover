#pragma once

#ifdef USE_FLASH

#include <Arduino.h>
#include "Utils/FileIO/BaseFileIO.hpp"

class FlashIO : public BaseFileIO {
public:
	FlashIO();

	bool begin();

	// State管理
	bool readState(int &state);
	bool writeState(const int &state);

protected:
	const char *getBasePath() const override;
};

#endif // USE_FLASH