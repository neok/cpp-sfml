#include <iostream>
#include <SFML/Graphics.hpp>

#include "Game.h"
#include "Vec2.h"
#include "Components.h"

int main() {
    Game game("config.txt");
    game.run();
}

