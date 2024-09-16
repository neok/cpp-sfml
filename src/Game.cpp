
#include <iostream>
#include <string>
#include <utility>
#include "Game.h"


Game::Game(const std::string & config) {
    init(config);
}

void Game::init(const std::string &config) {
    std::cout << "Game inited: " << config << std::endl;
}

void Game::run() {
    std::cout << "Game running" << std::endl;
}



