#pragma once

#include "Entity.h"
#include "EntityManager.h"
#include <SFML/Graphics.hpp>
#ifndef GAME_H
#define GAME_H

struct PlayerConfig {
    float size;
    int radius;
    float color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
};


const float LIGHTNING_RANGE = 250.0f;  // Maximum distance between chained enemies
const float LIGHTNING_DURATION = 0.2f; // Duration of each lightning segment


class Game {
    sf::RenderWindow m_window;
    EntityManager    m_entities;
    sf::Font         m_font;
    sf::Text         m_text;
    PlayerConfig     m_playerConfig{};
    int              m_score = 0;
    int              m_currentFrame = 0;
    int              m_lastEnemySpawnTime = 0;
    bool             m_paused = false; //whether we update game logic
    bool             m_running = true; // whether game is running
    int              m_enemySpawnCooldown = 20;

    // todo move into a separate config
    sf::Clock        m_deltaClock;
    sf::Clock        m_bfgCooldown;
    sf::Clock        m_lightningClock;
    sf::Vector2f     m_lightningStartPos;
    sf::VertexArray  m_lightningChain = sf::VertexArray(sf::Lines);
    bool             m_isLightningActive = false;
    bool             m_chainLightningFinished = false;
    std::shared_ptr<Entity> m_player;


    void init(const std::string &config);
    void setPaused(bool paused);

    void sMovement();      // System: Entity position / mov updated
    bool checkCollision(const sf::Vector2f &pos1, float radius1, const sf::Vector2f &pos2, float radius2);

    void sUserInput();     // System: User input
    void sLifespan();      // System: lifespan
    void sRender();        // System: render /drawing
    void renderEntity(std::shared_ptr<Entity> e);

    void sEnemySpawner();  // System: spawn enemies
    void sCollision();     // System: collisions

    void showScore();
    void spawnPlayer();

    void sGUI();

    void spawnEnemy();
    void spawnEnemyParticles(std::shared_ptr<Entity> entity, float lifeSpan);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos);
    void useUltimate(sf::Vector2f startPos);
public:
    explicit Game(const std::string &config);

    void run();
};


#endif //GAME_H
