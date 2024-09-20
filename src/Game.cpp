#include <iostream>
#include <string>
#include <utility>
#include "Game.h"
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cmath> // for sqrt()
#include <stdlib.h>
float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

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
    // spawnEnemy();
}

void Game::run() {
    while (m_window.isOpen()) {
        m_entities.update();
        sf::Event event;

        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        // sGUI()

        sRender();
        m_currentFrame++;
    }
}

void Game::sCollision() {
    for (auto & p : m_entities.getEntities("player")) {
        sf::Vector2f position = p->cShape->circle.getPosition();
        float radius = p->cShape->circle.getRadius();
        if (position.y - radius < 0) {
            std::cout << "collided" << std::endl;
            position.y = radius;
            p->cShape->circle.setPosition(position);
        }

    }

    for (auto & bullet : m_entities.getEntities("bullet")) {
        float bulletRadius = bullet->cShape->circle.getRadius();
        bullet->cShape->circle.setOrigin(bulletRadius, bulletRadius);
        sf::Vector2f bulletPos = bullet->cShape->circle.getPosition();


        for (auto& enemy : m_entities.getEntities("enemy")) {
            float enemyRadius = enemy->cShape->circle.getRadius();
            enemy->cShape->circle.setOrigin(enemyRadius, enemyRadius);

            sf::Vector2f enemyPos = enemy->cShape->circle.getPosition();
            float a = bulletPos.x - enemyPos.x;
            float b = bulletPos.y - enemyPos.y;
            float distanceSquared = (a * a) + (b * b);
            float radiusSum = bulletRadius + enemyRadius;

            if (distanceSquared <= (radiusSum * radiusSum)) {
                // Collision detected
                std::cout << "Collided" << std::endl;
                // bullet->destroy();
                enemy->destroy();
                // bullet->destroy();
            }
        }
    }


    auto windowSizeY = m_window.getSize().y;
    auto windowSizeX = m_window.getSize().x;

    for (auto& e : m_entities.getEntities("enemy")) {
        sf::Vector2f position = e->cShape->circle.getPosition();
        float radius = e->cShape->circle.getRadius();

        // Check and resolve collisions
        bool collided = false;

        // Bottom collision
        if (position.y + radius > windowSizeY) {
            position.y = windowSizeY - radius;
            e->cTransform->velocity.y = -std::abs(e->cTransform->velocity.y);
            collided = true;
        }
        // Top collision
        else if (position.y - radius < 0) {
            position.y = radius;
            e->cTransform->velocity.y = std::abs(e->cTransform->velocity.y);
            collided = true;
        }

        // Right collision
        if (position.x + radius > windowSizeX) {
            position.x = windowSizeX - radius;
            e->cTransform->velocity.x = -std::abs(e->cTransform->velocity.x);
            collided = true;
        }
        // Left collision
        else if (position.x - radius < 0) {
            position.x = radius;
            e->cTransform->velocity.x = std::abs(e->cTransform->velocity.x);
            collided = true;
        }

        // If collision occurred, update position and add a small random velocity change
        if (collided) {
            e->cShape->circle.setPosition(position);

            // Add a small random change to velocity to help prevent rhythmic bouncing
            // float randomFactor = 0.1f;  // Adjust this value to control the randomness
            // e->cTransform->velocity.x += randomFactor * (std::rand() % 100 - 50) / 50.0f;
            // e->cTransform->velocity.y += randomFactor * (std::rand() % 100 - 50) / 50.0f;
        }
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos) {
    auto bullet = m_entities.addEntity("bullet");
    sf::Vector2f playerPos = entity->cShape->circle.getPosition();
    Vec2 normalized = mousePos.normalized(playerPos);
    Vec2 velocity = Vec2(15.01f * normalized.x, 15.01f * normalized.y);



    bullet->cTransform = std::make_shared<CTransform>(Vec2(playerPos.x, playerPos.y), velocity, 0.0f);

    bullet->cShape = std::make_shared<CShape>(12.0f, 30, sf::Color(90, 0, 95), sf::Color(255, 255, 255), 1.0f);

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

    auto radius = m_player->cShape->circle.getRadius();
    m_player->cShape->circle.setOrigin(radius, radius);

    sf::Vector2f playerPos = m_player->cShape->circle.getPosition();

    float defaultRadius = 24.f;
    float constRange = 30.f;
    // do not spawn enemy on top of player
    // algo can be improved in many ways, but for this game it's more than enough
    if (ex > playerPos.x - radius && ex < playerPos.x + radius) {
        ex = playerPos.x + radius + constRange;
        if (ex + defaultRadius > static_cast<float>(m_window.getSize().x)) {
            ex = playerPos.x - radius - constRange;
        }
    }
    if (ey > playerPos.y - radius && ey < playerPos.y + radius) {
        ey = playerPos.y + radius + constRange;
        if (ey + defaultRadius > static_cast<float>(m_window.getSize().y)) {
            ey = playerPos.y - radius - constRange;
        }
    }

    float randVelX = -1.f;
    float randVelY = 1.f;
    entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(randVelX, randVelY), 0.0f);

    entity->cShape = std::make_shared<CShape>(defaultRadius, 10, sf::Color(100, 100, 100), sf::Color(255, 255, 255), 2.0f);

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

        if (event.type == sf::Event::MouseButtonPressed) {
            std::cout << event.mouseButton.button << std::endl;
            if (event.mouseButton.button == sf::Mouse::Left) {
                std::cout << "spawn bullet" << std::endl;

                spawnBullet(m_player, Vec2(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)));
            }
        }
    }
}


void Game::sMovement() {
    // Update the position of all entities based on their velocity
    for (auto &e: m_entities.getEntities("bullet")) {
        e->cTransform->pos = e->cTransform->pos + e->cTransform->velocity;
    }
    for (auto &e: m_entities.getEntities("enemy")) {
        e->cTransform->pos = e->cTransform->pos + e->cTransform->velocity;
    }
    const sf::CircleShape& playerCircle = m_player->cShape->circle;
    if (m_player->cInput->down) {
        if (playerCircle.getPosition().y + playerCircle.getRadius() < m_window.getSize().y) {
            m_player->cTransform->pos.y += 4.0f;
        }
    }
    if (m_player->cInput->left) {
        if (playerCircle.getPosition().x - playerCircle.getRadius() > 0.0f) {
           m_player->cTransform->pos.x -= 4.0f;
        }

    }
    if (m_player->cInput->up) {
        if (playerCircle.getPosition().y - playerCircle.getRadius() > 0) {
            m_player->cTransform->pos.y -= 4.0f;
        }
    }
    if (m_player->cInput->right) {
        if (playerCircle.getPosition().x + playerCircle.getRadius() < m_window.getSize().x) {
            m_player->cTransform->pos.x += 4.0f;
        }
    }
}

