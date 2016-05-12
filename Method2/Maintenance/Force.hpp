#ifndef FORCE_HPP
#define FORCE_HPP

#include <cmath>

const double PI = 3.14159265;
const double DIFFERENCE_THRESHOLD = 1.0;
struct Force {
	double x;				// Force in x-direction
	double y;				// Force in y-direction

	Force(double x = 0.0, double y = 0.0): x(x), y(y) {}
	Force(double x, double y, double magnitude);
	Force(const Force&);
	~Force() {}
	double magnitude();		// Calculates and returns the magnitude of the force
	friend Force operator+(const Force& f1, const Force& f2);
	friend Force operator-(const Force& f1, const Force& f2);
	Force& operator+=(const Force& f);
	Force& operator-=(const Force& f);
	friend Force operator/(const Force& f, const double d);
	friend Force operator*(const Force& f, const double d);
	double dot(Force&);
	double angle(Force&);
};

const Force null_force(0.0,0.0);
#endif