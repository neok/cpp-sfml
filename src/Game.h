#pragma once

#include "Entity.h"
#include "EntityManager.h"
#include <SFML/Graphics.hpp>
#ifndef GAME_H
#define GAME_H

struct PlayerConfig {
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V;
    float S;
};

struct EnemyConfig {
    int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
    float SMIN, SMAX;
};

struct BulletConfig {
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;
    float S;
};

class Game {
    sf::RenderWindow m_window;
    EntityManager    m_entities;
    sf::Font         m_font;
    sf::Text         m_text;
    PlayerConfig     m_playerConfig{};
    EnemyConfig      m_enemyConfig{};
    BulletConfig     m_bulletConfig{};
    int              m_score = 0;
    int              m_currentFrame = 0;
    int              m_lastEnemySpawnTime = 0;
    bool             m_paused = false; //whether we update game logic
    bool             m_running = true; // whether game is running
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
    void spawnEnemy();
    void spawnEnemyParticles(std::shared_ptr<Entity> entity);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos);
    void spawnBFG(std::shared_ptr<Entity> entity);
public:
    explicit Game(const std::string &config);

    void run();
};


#endif //GAME_H
