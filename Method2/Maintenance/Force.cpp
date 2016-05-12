#include "Force.hpp"

Force::Force(double x, double y, double magnitude) {
	// x and y specify the direction and magnitude specifies the desired magnitude of the force in that direction
	this->x = x;
	this->y = y;
	double current_mag = this->magnitude();
	double scale = magnitude / current_mag;
	this->x *= scale;
	this->y *= scale;
}

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

double Force::dot(Force& f) {
	return x*f.x + y*f.y;
}

double Force::angle(Force& f) {
	double dot_product = this->dot(f);
	double param = dot_product / this->magnitude() / f.magnitude();
	return (acos(param) * 180.0 / PI);
}