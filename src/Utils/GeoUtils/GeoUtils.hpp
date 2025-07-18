#pragma once

namespace GeoUtils {
	constexpr double EARTH_RADIUS = 6371000.0;
	double toRadians(double degrees);
	double toDegrees(double radians);
	double haversineDistance(double latitude1, double longitude1, double latitude2, double longitude2);
	double haversineBearing(double latitude1, double longitude1, double latitude2, double longitude2);
}