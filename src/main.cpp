#include <iostream>
#include <SFML/Graphics.hpp>

#include "Game.h"
#include "Vec2.h"
#include "Components.h"

int main() {
    const Vec2 v1(10, 10);
    v1.print();
    const sf::Color color = sf::Color::White;

    const CShape cc(10, 1, color, color, 2);
    std::cout << cc.circle.getOrigin().x  << cc.circle.getOrigin().y << std::endl;
    Game game("config.txt");
    game.run();
}

