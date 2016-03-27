#ifndef FORCE_HPP
#define FORCE_HPP

struct Force {
	double x;				// Force in x-direction
	double y;				// Force in y-direction

	Force(double x = 0.0, double y = 0.0): x(x), y(y) {}
	Force(const Force&)
	~Force() {}
	inline double magnitude();		// Calculates and returns the magnitude of the force
	Force operator+(const Force& f1, const Force& f2);
	Force operator-(const Force& f1, const Force& f2);
	Force operator/(const Force& f, const double d);
	Force operator*(const Force& f, const double d);
};

#endif