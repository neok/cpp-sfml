#include <iostream>
#include <string>
#include <utility>
#include "Game.h"
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cmath>
#include <random>

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

sf::Color getRandomColor() {
    int red = rand() % 256;
    int green = rand() % 256;
    int blue = rand() % 256;

    return sf::Color(red, green, blue);
}

Game::Game(const std::string &config) {
    init(config);
}

void Game::spawnEnemyParticles(std::shared_ptr<Entity> entity) {
    sf::Vector2f position = entity->cShape->circle.getPosition();
    unsigned int pointCount = entity->cShape->circle.getPointCount();
    float radius = entity->cShape->circle.getRadius() / 3;

    float angleStep = 360.0f / pointCount;

    for (unsigned int i = 0; i < pointCount; i++) {
        auto particle = m_entities.addEntity("particle");
        float angle = i * angleStep * (3.14159f / 180.0f); // Convert degrees to radians
        float dirX = std::cos(angle);
        float dirY = std::sin(angle);

        particle->cTransform = std::make_shared<CTransform>(
            Vec2(position.x, position.y),
            Vec2(dirX, dirY),
            1.0f
        );

        particle->cShape = std::make_shared<CShape>(
            radius,
            pointCount,
            entity->cShape->circle.getFillColor(),
            entity->cShape->circle.getOutlineColor(),
            1.0f);
        particle->cLifespan = std::make_shared<CLifespan>(130.0f);
    }
}

void Game::init(const std::string &config) {
    // std::ifstream fin(config);
    //
    // fin >> m_window.

    sf::Font myfont;
    if (!myfont.loadFromFile("../fonts/arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        exit(-1);
    }
    m_font = myfont;
    m_window.create(sf::VideoMode(1600, 900), "Ownage", sf::Style::Default);
    m_window.setFramerateLimit(60);

    spawnPlayer();
    showScore();

}

void Game::run() {
    while (m_window.isOpen()) {
        m_entities.update();
        sf::Event event;

        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        sLifespan();
        // sGUI()

        sRender();
        m_currentFrame++;
    }
}

void Game::sCollision() {


    for (auto &bullet: m_entities.getEntities("bullet")) {
        float bulletRadius = bullet->cShape->circle.getRadius();
        bullet->cShape->circle.setOrigin(bulletRadius, bulletRadius);
        sf::Vector2f bulletPos = bullet->cShape->circle.getPosition();


        for (auto &enemy: m_entities.getEntities("enemy")) {
            float enemyRadius = enemy->cShape->circle.getRadius();
            enemy->cShape->circle.setOrigin(enemyRadius, enemyRadius);

            sf::Vector2f enemyPos = enemy->cShape->circle.getPosition();

            if (checkCollision(enemyPos, enemyRadius, bulletPos, bulletRadius)) {
                // Collision detected
                std::cout << "Collided" << std::endl;
                enemy->destroy();
                spawnEnemyParticles(enemy);
                bullet->destroy();
                m_player->cScore->score += 100;
            }
        }
    }


    auto windowSizeY = static_cast<float>(m_window.getSize().y);
    auto windowSizeX = static_cast<float>(m_window.getSize().x);

    for (auto &e: m_entities.getEntities("enemy")) {
        sf::Vector2f position = e->cShape->circle.getPosition();
        float radius = e->cShape->circle.getRadius();

        bool collided = false;

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

        if (position.x + radius > windowSizeX) {
            position.x = windowSizeX - radius;
            e->cTransform->velocity.x = -std::abs(e->cTransform->velocity.x);
            collided = true;
        }
        else if (position.x - radius < 0) {
            position.x = radius;
            e->cTransform->velocity.x = std::abs(e->cTransform->velocity.x);
            collided = true;
        }

        if (collided) {
            e->cShape->circle.setPosition(position);

        }
    }

    for (auto &p: m_entities.getEntities("player")) {
        float playerRadius = p->cShape->circle.getRadius();
        p->cShape->circle.setOrigin(playerRadius, playerRadius);
        sf::Vector2f playerPosition = p->cShape->circle.getPosition();
        for (auto &enemy : m_entities.getEntities("enemy")) {
            float enemyRadius = enemy->cShape->circle.getRadius();
            enemy->cShape->circle.setOrigin(enemyRadius, enemyRadius);
            sf::Vector2f enemyPos = enemy->cShape->circle.getPosition();

            if (checkCollision(playerPosition, playerRadius, enemyPos, enemyRadius)) {

                spawnEnemyParticles(enemy);
                enemy->destroy();

                //set pos middle again, maybe add death counter
                p->cTransform->pos.x = static_cast<float>(m_window.getSize().x) / 2.0f;
                p->cTransform->pos.y = static_cast<float>(m_window.getSize().y) / 2.0f;
            }
        }
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos) {
    auto bullet = m_entities.addEntity("bullet");
    sf::Vector2f playerPos = entity->cShape->circle.getPosition();
    Vec2 normalized = mousePos.normalized(playerPos);
    Vec2 velocity = Vec2(14.01f * normalized.x, 14.01f * normalized.y);


    bullet->cLifespan = std::make_shared<CLifespan>(CLifespan(150));
    bullet->cTransform = std::make_shared<CTransform>(Vec2(playerPos.x, playerPos.y), velocity, 0.0f);

    bullet->cShape = std::make_shared<CShape>(12.0f, 30, sf::Color(90, 155, 95), sf::Color(255, 255, 255), 0.0f);
}

void Game::sEnemySpawner() {
    if (m_currentFrame - m_lastEnemySpawnTime <= 20) {
        return;
    }
    spawnEnemy();
}


void Game::spawnEnemy() {
    auto entity = m_entities.addEntity("enemy");
    float ex = rand() % m_window.getSize().x;
    float ey = rand() % m_window.getSize().y;

    const auto radius = m_player->cShape->circle.getRadius();
    m_player->cShape->circle.setOrigin(radius, radius);

    sf::Vector2f playerPos = m_player->cShape->circle.getPosition();



    float defaultRadius = RandomFloat(25, 55);
    float constRange = 100.f;
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

    float randVelX = getRandomFloatInRange(-3, 4);
    float randVelY = getRandomFloatInRange(-3, 2);
    entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(randVelX, randVelY), 2.0f);
    entity->cShape = std::make_shared<CShape>(defaultRadius, getRandomIntInRange(3, 10), getRandomColor(), getRandomColor(), 1.0f);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::showScore() {
    sf::Text text;
    text.setFont(m_font); // font is a sf::Font
    text.setString("Score: ");
    text.setCharacterSize(24); // in pixels, not points!
    text.setFillColor(sf::Color(255, 255, 255));
    text.setStyle(sf::Text::Bold);

    m_text = text;
}


void Game::spawnPlayer() {
    auto entity = m_entities.addEntity("player");
    float mx = static_cast<float>(m_window.getSize().x) / 2.0f;
    float my = static_cast<float>(m_window.getSize().y) / 2.0f;
    entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), 0.0f);

    entity->cShape = std::make_shared<CShape>(32.0f, 6, sf::Color(99, 155, 99), sf::Color(0, 255, 100), 2.0f);

    entity->cInput = std::make_shared<CInput>();
    entity->cScore = std::make_shared<CScore>(0);

    m_player = entity;
}

void Game::sRender() {
    m_window.clear(sf::Color::Black);
    m_text.setPosition(130, 130);
    m_text.setString("Score: " + std::to_string(m_player->cScore->score));
    m_window.draw(m_text);
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
    sf::Event event{};
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
                default:
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
                default:
                    break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            std::cout << event.mouseButton.button << std::endl;
            if (event.mouseButton.button == sf::Mouse::Left) {
                spawnBullet(m_player, Vec2(static_cast<float>(event.mouseButton.x),
                                           static_cast<float>(event.mouseButton.y)));
            }
        }
    }
}

void Game::sLifespan() {
    for (auto &e: m_entities.getEntities()) {
        if (e->cLifespan != nullptr) {
            e->cLifespan->remaining -= 1;

            if (e->cLifespan->remaining <= 0) {
                e->destroy();
                continue;
            }
            float lifespanRatio = static_cast<float>(e->cLifespan->remaining) / static_cast<float>(e->cLifespan->total);

            float alpha = 255 * lifespanRatio;
            alpha = std::max(alpha, 0.0f);

            const sf::Color color = e->cShape->circle.getFillColor();
            e->cShape->circle.setFillColor(sf::Color(color.r, color.g, color.b, static_cast<sf::Uint8>(alpha)));
            const sf::Color outlineColor = e->cShape->circle.getOutlineColor();
            e->cShape->circle.setOutlineColor(sf::Color(outlineColor.r, outlineColor.g, outlineColor.b, static_cast<sf::Uint8>(alpha)));
        }
    }
}


void Game::sMovement() {
    for (auto &e: m_entities.getEntities("particle")) {
        e->cTransform->pos = e->cTransform->pos + e->cTransform->velocity;
    }
    // Update the position of all entities based on their velocity
    for (auto &e: m_entities.getEntities("bullet")) {
        e->cTransform->pos = e->cTransform->pos + e->cTransform->velocity;
    }
    for (auto &e: m_entities.getEntities("enemy")) {
        e->cTransform->pos = e->cTransform->pos + e->cTransform->velocity;
    }
    const sf::CircleShape &playerCircle = m_player->cShape->circle;
    if (m_player->cInput->down) {
        if (playerCircle.getPosition().y + playerCircle.getRadius() < static_cast<float>(m_window.getSize().y)) {
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
        if (playerCircle.getPosition().x + playerCircle.getRadius() < static_cast<float>(m_window.getSize().x)) {
            m_player->cTransform->pos.x += 4.0f;
        }
    }
}

bool Game::checkCollision(const sf::Vector2f& pos1, float radius1, const sf::Vector2f& pos2, float radius2) {
    float a = pos1.x - pos2.x;
    float b = pos1.y - pos2.y;

    float distanceSquared = (a * a) + (b * b);
    float radiusSum = radius1 + radius2;

    return distanceSquared <= (radiusSum * radiusSum);
}