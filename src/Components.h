#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#ifndef COMPONENTS_H
#define COMPONENTS_H

class CTransform {
public:
    Vec2 pos = {0, 0};
    Vec2 velocity = {0, 0};
    float angle = 0;

    CTransform(const Vec2& pos, const Vec2 &v, float angle): pos(pos), velocity(v), angle(angle) {
    }
};

class CShape {
public:
    sf::CircleShape circle;

    CShape(float radius, int points, const sf::Color &fill, const sf::Color &outline, float thikness): circle(
        radius, points) {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thikness);
        circle.setOrigin(radius, radius);
    }
};

class CCollision {
public:
    float radius = 0;
    explicit CCollision(const float radius): radius(radius) {}
};

class CScore {
    public:
    int score = 0;
    explicit CScore(const int score): score(score) {}
};

class CLifespan {
    public:
    int remaining = 0; // amount of lifespan remaining on the entity
    int total = 0; // total initial amount of lifespan
    explicit CLifespan(const int total) : remaining(total), total(total) {}
};

class CInput {
    public:
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool shoot = false;
    CInput() {}
};

#endif
