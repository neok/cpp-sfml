
#ifndef UTILS_H
#define UTILS_H
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
float RandomFloat(float a, float b);
float getRandomFloatInRange(float min, float max);
int getRandomIntInRange(int min, int max);
float distance(sf::Vector2f p1, sf::Vector2f p2);
sf::Color getRandomColor();
#endif //UTILS_H
