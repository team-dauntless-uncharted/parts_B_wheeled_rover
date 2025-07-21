#pragma once

#include <GNSS.h>

class GnssSensor {
public:
	GnssSensor();
	bool begin();
	void update();
	double getLatitude() const;
	double getLongitude() const;
	double getAltitude() const;
	char *getCurrentDate() const;
	bool isPosFix() const;

private:
	SpGnss _gnss;
	double _latitude;
	double _longitude;
	double _altitude;
	bool _posFix;
	static const int STRING_BUFFER_SIZE = 128;
	char _currentDate[STRING_BUFFER_SIZE];
};