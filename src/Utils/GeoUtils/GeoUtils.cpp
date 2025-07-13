#include "GeoUtils.hpp"

namespace GeoUtils {
	double toRadians(double degrees) {
		return degrees * M_PI / 180.0;
	}

	double toDegrees(double radians) {
		return radians * 180.0 / M_PI;
	}

	double haversineDistance(double latitude1, double longitude1, double latitude2, double longitude2) {
		double dLatitude = toRadians(latitude2 - latitude1);
		double dLongitude = toRadians(longitude2 - longitude1);

		double a = pow(sin(dLatitude / 2), 2) + cos(toRadians(latitude1)) * cos(toRadians(latitude2)) * pow(sin(dLongitude / 2), 2);
		double c = 2 * atan2(sqrt(a), sqrt(1 - a));

		return EARTH_RADIUS * c;
	}

	double haversineBearing(double latitude1, double longitude1, double latitude2, double longitude2) {
		double dLongitude = toRadians(longitude2 - longitude1);

		double y = sin(dLongitude) * cos(toRadians(latitude2));
		double x = cos(toRadians(latitude1)) * sin(toRadians(latitude2)) - sin(toRadians(latitude1)) * cos(toRadians(latitude2)) * cos(dLongitude);
		double bearing = atan2(y, x);

		return fmod((toDegrees(bearing) + 360.0), 360.0);
	}
}