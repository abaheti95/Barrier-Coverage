#ifndef COORDINATE_HPP
#define COORDINATE_HPP

#include <cmath>
#include <cstdlib>

using namespace std;

struct Coordinate
{
	double x;
	double y;

	Coordinate(double = 0.0, double = 0.0);
	Coordinate(const Coordinate&);
	~Coordinate() {}
	friend double coordinateDistance(const Coordinate&, const Coordinate&);
};

double distance(const Coordinate&, const Coordinate&);

struct XCoordinateComparator
{
	bool operator()(const Coordinate &left, const Coordinate &right)
	{
		return left.x < right.x;
	}
};

#endif
