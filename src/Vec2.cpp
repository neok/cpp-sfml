#include "Vec2.h"
#include <iostream>

void Vec2::print() const {
	std::cout << "x: " << x << ", y: " << y << std::endl;
}
Vec2 Vec2::operator + (const Vec2 &other) const {
	return Vec2(x + other.x, y + other.y);
}

