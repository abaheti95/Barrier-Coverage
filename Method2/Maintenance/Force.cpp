#include "Force.hpp"

Force::Force(const Force& f) {
	x = f.x;
	y = f.y;
}

double Force::magnitude() {
	return sqrt(x*x + y*y);
}

Force& Force::operator+=(const Force& f) {
	this->x += f.x;
	this->y += f.y;
	return *this;
}

Force& Force::operator-=(const Force& f) {
	this->x -= f.x;
	this->y -= f.y;
	return *this;
}

Force operator+(const Force& f1, const Force& f2) {
	return Force(f1.x + f2.x, f1.y + f2.y);
}

Force operator-(const Force& f1, const Force& f2) {
	return Force(f1.x - f2.x, f1.y - f2.y);
}

Force operator/(const Force& f, const double d) {
	return Force(f.x / d , f.y / d);
}

Force operator*(const Force& f, const double d) {
	return Force(f.x * d , f.y * d);
}
