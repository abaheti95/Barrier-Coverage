#include "Coordinate.hpp"

Coordinate::Coordinate(double x, double y):x(x), y(y) {}

Coordinate::Coordinate(const Coordinate& copyCoordinate)
{
	x = copyCoordinate.x;
	y = copyCoordinate.y;
}

double coordinateDistance(const Coordinate& first, const Coordinate& second)
{
	double diffX = first.x - second.x;
	double diffY = first.y - second.y;
	return sqrt(diffX * diffX + diffY * diffY);
}

double euclidean_distance(const Coordinate& first, const Coordinate& second)
{
	double diffX = first.x - second.x;
	double diffY = first.y - second.y;
	return sqrt(diffX * diffX + diffY * diffY);
}

double distance(const Coordinate& first, const Coordinate& second) {
	return euclidean_distance(first, second);
}
