#include <iostream>
#include <string>
#include <utility>
#include "Game.h"
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <stdlib.h>


Game::Game(const std::string &config) {
    init(config);
}

void Game::init(const std::string &config) {
    // std::ifstream fin(config);
    //
    // fin >> m_window.


    m_window.create(sf::VideoMode(1280, 720), "Ownage");
    m_window.setFramerateLimit(60);

    spawnPlayer();
}

void Game::run() {
    while (m_window.isOpen()) {
        m_entities.update();
        sf::Event event;

        sEnemySpawner();

        // sCollision();

        sUserInput();
        sMovement();
        sRender();
    }
}

void Game::sEnemySpawner() {
    if (m_currentFrame - m_lastEnemySpawnTime <= 100) {
        return;
    }
    spawnEnemy();
}


void Game::spawnEnemy() {
    auto entity = m_entities.addEntity("enemy");
    float ex = rand() % m_window.getSize().x;
    float ey = rand() % m_window.getSize().y;

    entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(0.0f, 0.0f), 1.0f);

    entity->cShape = std::make_shared<CShape>(14.0f, 3, sf::Color(100, 100, 100), sf::Color(255, 255, 255), 2.0f);

    m_lastEnemySpawnTime = m_currentFrame;
}


void Game::spawnPlayer() {
    auto entity = m_entities.addEntity("player");
    float mx = m_window.getSize().x / 2.0f;
    float my = m_window.getSize().y / 2.0f;
    entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), 0.0f);

    entity->cShape = std::make_shared<CShape>(32.0f, 6, sf::Color(255, 0, 255), sf::Color(0, 255, 100), 2.0f);

    entity->cInput = std::make_shared<CInput>();

    m_player = entity;
}

void Game::sRender() {
    m_window.clear(sf::Color::Black);
    // std::cout << "Entities size" << m_entities.getEntities().size() << std::endl;
    for (auto &e: m_entities.getEntities()) {
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
        e->cTransform->angle += 0.8f;

        e->cShape->circle.setRotation(e->cTransform->angle);
        m_window.draw(e->cShape->circle);
    }
    m_currentFrame++;
    m_window.display();
}

void Game::sUserInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::W:

                    m_player->cInput->up = true;
                    break;
                case sf::Keyboard::D:

                    m_player->cInput->right = true;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = true;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = true;
                    break;
            }
        }

        if (event.type == sf::Event::KeyReleased) {
            switch (event.key.code) {
                case sf::Keyboard::W:

                    m_player->cInput->up = false;
                    break;
                case sf::Keyboard::D:

                    m_player->cInput->right = false;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = false;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = false;
                    break;
            }
        }
    }
}


void Game::sMovement() {
    // Update the position of all entities based on their velocity
    for (auto e: m_entities.getEntities("enemy")) {
        e->cTransform->pos = e->cTransform->pos + e->cTransform->velocity;
    }
    if (m_player->cInput->down) {
        m_player->cTransform->pos.y += 4.0f;
    }
    if (m_player->cInput->left) {
        m_player->cTransform->pos.x -= 4.0f;
    }
    if (m_player->cInput->up) {
        m_player->cTransform->pos.y -= 4.0f;
    }
    if (m_player->cInput->right) {
        m_player->cTransform->pos.x += 4.0f;
    }
}
