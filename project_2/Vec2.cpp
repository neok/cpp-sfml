#include "Vec2.h"
#include <iostream>
#include <cmath>
#include <SFML/System/Vector2.hpp>

void Vec2::print() const {
	std::cout << "x: " << x << ", y: " << y << std::endl;
}
Vec2 Vec2::operator + (const Vec2 &other) const {
	return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::normalized(const Vec2 &other) const {
	auto distance = Vec2(x - other.x, y - other.y);
	const float magnitude = std::sqrt(distance.x * distance.x + distance.y * distance.y);

	return Vec2(distance.x / magnitude, distance.y / magnitude);
}

Vec2 Vec2::normalized(const sf::Vector2f &other) const {
	auto distance = Vec2(x - other.x, y - other.y);
	const float magnitude = std::sqrt(distance.x * distance.x + distance.y * distance.y);

	return Vec2(distance.x / magnitude, distance.y / magnitude);
}

