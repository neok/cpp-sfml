#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <random>
#include "Utils.h"


float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

float getRandomFloatInRange(float min, float max) {
    static std::random_device rd;
    static std::mt19937 generator(rd());

    std::uniform_real_distribution<float> distribution(min, max);
    float randomFloat = distribution(generator);

    return std::round(randomFloat * 100.0f) / 100.0f;
}

int getRandomIntInRange(int min, int max) {
    static std::random_device rd;
    static std::mt19937 generator(rd());

    std::uniform_int_distribution<int> distribution(min, max);

    return distribution(generator);
}

float distance(sf::Vector2f p1, sf::Vector2f p2) {
    return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

sf::Color getRandomColor() {
    int red = rand() % 256;
    int green = rand() % 256;
    int blue = rand() % 256;

    return sf::Color(red, green, blue);
}

