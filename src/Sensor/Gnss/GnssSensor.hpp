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
	String getCurrentDate() const;
	bool isPosFix() const;

private:
	SpGnss _gnss;
	double _latitude;
	double _longitude;
	double _altitude;
	String _currentDate;
	bool _posFix;
	static const int STRING_BUFFER_SIZE = 128;

	String createDate(SpGnssTime time);
};