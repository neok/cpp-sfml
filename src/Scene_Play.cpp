#include <iostream>

#include "SFML//Window/Event.hpp"
//#include "Scene_Menu.h"
#include "Scene_Play.h"
#include "Assets.h"
// #include "Physics.h"
#include <cmath>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <fstream>
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "Physics.h"


Scene_Play::Scene_Play(GameEngine *gameEngine, const std::string &levelPath)
    : Scene(gameEngine), m_levelPath(levelPath) {
    init(levelPath);
}

void Scene_Play::init(const std::string &levelPath) {
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE"); // Toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); // Toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::G, "TOGGLE_GRID"); // Toggle drawing (G)rid
    registerAction(sf::Keyboard::D, "MOVE_RIGHT");
    registerAction(sf::Keyboard::A, "MOVE_LEFT");
    registerAction(sf::Keyboard::W, "JUMP");
    registerAction(sf::Keyboard::Space, "SHOOT");

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Mario"));
    // m_gridText.setFont(m_game->assets().getFont("Tech"));

    loadLevel(levelPath);
}

vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) {
    // TODO: This function takes in a grid (x,y) position and an Entity
    //       Return a vec2 indicating where the CENTER position of the Entity should be
    //       You must use the Entity's Animation size to position it correctly
    //       The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y
    //       The bottom-left corner of the Animation should aligh with the bottom left of the grid cell
    auto entitySize = entity->getComponent<CAnimation>().animation.getSize();

    return {
        gridX * m_gridSize.x + entitySize.x / 2.0f,
        static_cast<float>(height()) - gridY * m_gridSize.y - entitySize.y / 2.0f
    };
}

void Scene_Play::loadLevel(const std::string &fileName) {
    m_entityManager = EntityManager();

    // read in the level file and add the appropriate entities
    // use the PlayerConfig struct m_playerConfig to store player properties
    // this struct is defined at the top of Scene_Play.h
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Scene_Play::loadLevel could not load " << fileName << " file.\n";
        exit(-1);
    }

    std::string entityType;
    while (file >> entityType) {
        std::cout << entityType << std::endl;

        if (entityType == "Tile") {
            std::string animationName;
            float gridX, gridY;
            file >> animationName >> gridX >> gridY;

            auto tile = m_entityManager.addEntity("tile");
            tile->addComponent<CAnimation>(m_game->assets().getAnimation(animationName), true);
            tile->addComponent<CTransform>(
                gridToMidPixel(gridX, gridY, tile),
                vec2(0, 0),
                vec2(1, 1),
                0
            );
            tile->addComponent<CBoundingBox>(m_gridSize);
        } else if (entityType == "Dec") {
            std::string animationName;
            float gridX, gridY;
            file >> animationName >> gridX >> gridY;

            auto dec = m_entityManager.addEntity("dec");
            dec->addComponent<CAnimation>(m_game->assets().getAnimation(animationName), true);
            dec->addComponent<CTransform>(
                gridToMidPixel(gridX, gridY, dec),
                vec2(0, 0),
                vec2(1, 1),
                0
            );
        } else if (entityType == "Player") {
            file >> m_playerConfig.X >> m_playerConfig.Y
                    >> m_playerConfig.CX >> m_playerConfig.CY
                    >> m_playerConfig.SPEED
                    >> m_playerConfig.JUMP
                    >> m_playerConfig.MAX_SPEED
                    >> m_playerConfig.GRAVITY
                    >> m_playerConfig.WEAPON;
            spawnPlayer();
        } else {
            std::cerr << "Unknown entity type " << entityType << "\n";
            // exit(-1);
        }
    }
}

void Scene_Play::spawnPlayer() {
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    m_player->addComponent<CTransform>(
        gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player),
        vec2(m_playerConfig.SPEED, 0),
        vec2(1, 1),
        0
    );
    m_player->addComponent<CBoundingBox>(vec2(m_playerConfig.CX, m_playerConfig.CY));
    m_player->addComponent<CInput>();
    m_player->addComponent<CState>("stand");
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity) {
    // TODO: this should spawn a bullet at the given entity, going in the direction the entity is facing

    auto bullet = m_entityManager.addEntity("bullet");
    bullet->addComponent<CAnimation>(m_game->assets().getAnimation(m_playerConfig.WEAPON), true);
    auto direction = entity->getComponent<CTransform>().scale.x < 0 ? -1 : 1;
    bullet->addComponent<CTransform>(
        entity->getComponent<CTransform>().pos,
        vec2(direction * 2 * m_playerConfig.SPEED, 0),
        entity->getComponent<CTransform>().scale,
        0
    );
    bullet->addComponent<CBoundingBox>(bullet->getComponent<CAnimation>().animation.getSize());
    bullet->addComponent<CLifespan>(100, m_currentFrame);
}

void Scene_Play::update() {
    m_entityManager.update();

    if (!m_paused) {
        sMovement();
        sLifespan();
        sCollision();
        sAnimation();
    } else {
        //debug
        // for (const auto &tile: m_entityManager.getEntities("tile")) {
        //     auto& tilePos = tile->getComponent<CTransform>().pos;
        //     auto playerPos = m_player->getComponent<CTransform>().pos;
        //     const vec2 overlap = Physics::GetOverlap(m_player, tile);
        //     const vec2 pOverlap = Physics::GetPreviousOverlap(m_player, tile);
        //     if (overlap.y < 0 && overlap.x > 0) {
        //         std::cout << overlap.x << " " << overlap.y << std::endl;
        //     }
        // }
    }

    sRender();
    m_currentFrame++;
}

void Scene_Play::sMovement() {
    m_player->getComponent<CTransform>().velocity.x = 0;

    if (m_player->getComponent<CInput>().up && m_player->getComponent<CInput>().canJump) {
        m_player->getComponent<CTransform>().velocity.y -= m_playerConfig.JUMP;
        m_player->getComponent<CInput>().canJump = false;
    }

    if (m_player->getComponent<CInput>().right) {
        m_player->getComponent<CTransform>().velocity.x = m_playerConfig.SPEED;
        m_player->getComponent<CTransform>().scale.x = 1;
    }

    if (m_player->getComponent<CInput>().left) {
        m_player->getComponent<CTransform>().velocity.x = -m_playerConfig.SPEED;
        m_player->getComponent<CTransform>().scale.x = -1;
    }
    if (m_player->getComponent<CInput>().shoot) {
        if (m_player->getComponent<CInput>().canShoot) {
            spawnBullet(m_player);
            m_player->getComponent<CInput>().canShoot = false;
        }
    } else {
        m_player->getComponent<CInput>().canShoot = true;
    }

    for (auto& entity : m_entityManager.getEntities()) {
        if (entity->hasComponent<CGravity>()) {
            vec2 &v = entity->getComponent<CTransform>().velocity;
            v.y += entity->getComponent<CGravity>().gravity;
            if (v.y > m_playerConfig.MAX_SPEED) {
                v.y = m_playerConfig.MAX_SPEED;
            }
        }
        entity->getComponent<CTransform>().prevPos = entity->getComponent<CTransform>().pos;
        entity->getComponent<CTransform>().pos += entity->getComponent<CTransform>().velocity;
    }
}

void Scene_Play::sLifespan() {
    for (const auto& entity : m_entityManager.getEntities()) {
        if (entity->hasComponent<CLifespan>()) {
            auto &eLife = entity->getComponent<CLifespan>();

            if (m_currentFrame - eLife.frameCreated >= eLife.lifespan) {
                entity->destroy();
            }
        }
    }
}

void Scene_Play::sCollision() {
    // REMEMBER: SFML's (0,0) position is in the TOP-LEFT corner
    //           This means jumping will have a negative y-component
    //           and gravity will have a positive y-component
    //           Also, something BELOW something else will hava a y value GREATER than it
    //           Also, something ABOVE something else will hava a y value LESS than it

    for (const auto &bullet: m_entityManager.getEntities("bullet")) {
        for (const auto &tile: m_entityManager.getEntities("tile")) {
            vec2 overlap = Physics::GetOverlap(bullet, tile);
            vec2 pOverlap = Physics::GetPreviousOverlap(bullet, tile);

            if (0 < overlap.y && -m_gridSize.x < overlap.x) {
                if (0 <= overlap.x && pOverlap.x <= 0) {
                    bullet->destroy();
                    tile->getComponent<CAnimation>().animation = m_game->assets().getAnimation("Explosion");
                    tile->addComponent<CLifespan>(12, m_currentFrame);
                    break;
                }
            }
        }
    }
     // auto& playerTransform = m_player->getComponent<CTransform>();
    // TODO: Implement player/tile collisions and resolutions
    //       Update the CState component of the player to store whether
    //       it is currently on the ground or in the air. This will be
    //       used by the Animation system
    m_player->getComponent<CGravity>().gravity = m_playerConfig.GRAVITY;
    for (const auto &tile: m_entityManager.getEntities("tile")) {
        const vec2 overlap = Physics::GetOverlap(m_player, tile);
        const vec2 pOverlap = Physics::GetPreviousOverlap(m_player, tile);
        float dy = tile->getComponent<CTransform>().pos.y - m_player->getComponent<CTransform>().pos.y;


        //check if play in the air
        // check if tiles below
        if (0 < overlap.x && -m_gridSize.y < overlap.y && dy > 0) {
            if (0 < overlap.y && pOverlap.y <= 0) {
                m_player->getComponent<CInput>().canJump = true;
                m_player->getComponent<CGravity>().gravity = 0;
                m_player->getComponent<CTransform>().velocity.y = 0;
                m_player->getComponent<CTransform>().pos.y -= overlap.y;
            }
        }

        //check if player hit brick from below
        if (overlap.x > 0 && -m_gridSize.y < overlap.y && dy < 0) {
            if (overlap.y >= 0 && pOverlap.y <= 0) {
                m_player->getComponent<CTransform>().pos.y += overlap.y;
                m_player->getComponent<CTransform>().velocity.y = 0;
            }
        }
     
        float dx = tile->getComponent<CTransform>().pos.x - m_player->getComponent<CTransform>().pos.x;
        if (0 < overlap.y && -m_gridSize.x < overlap.x) {
            if (0 <= overlap.x && pOverlap.x <= 0) {
                if (dx > 0) {
                    // tile is right of player
                    m_player->getComponent<CTransform>().pos.x -= overlap.x;
                } else {
                    // tile is left of player
                    m_player->getComponent<CTransform>().pos.x += overlap.x;
                }
            }
        }
    }

    //if player fall down
    if (m_player->getComponent<CTransform>().pos.y > static_cast<float>(height())) {
        m_player->getComponent<CTransform>().pos = gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player);
    }

    if (m_player->getComponent<CTransform>().pos.x < m_gridSize.x - 38.f) {
        m_player->getComponent<CTransform>().pos.x = m_gridSize.x - 38.f;
    }
}

void Scene_Play::sDoAction(const Action &action) {
    if (action.type() == "START") {
        if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; } else if (
            action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; } else if (
            action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; } else if (action.name() == "PAUSE") {
            setPaused(!m_paused);
        } else if (action.name() == "QUIT") { onEnd(); }
        else if (action.name() == "JUMP") {
            if (m_player->getComponent<CInput>().canJump) {
                m_player->getComponent<CInput>().up = true;
            }
        } else if (action.name() == "MOVE_RIGHT") {
            m_player->getComponent<CInput>().right = true;
        } else if (action.name() == "MOVE_LEFT") {
            m_player->getComponent<CInput>().left = true;
        } else if (action.name() == "SHOOT") {
            m_player->getComponent<CInput>().shoot = true;
        }
    } else if (action.type() == "END") {
        if (action.name() == "MOVE_RIGHT") {
            m_player->getComponent<CInput>().right = false;
        } else if (action.name() == "MOVE_LEFT") {
            m_player->getComponent<CInput>().left = false;
        } else if (action.name() == "JUMP") {
            m_player->getComponent<CInput>().up = false;
        } else if (action.name() == "SHOOT") {
            m_player->getComponent<CInput>().shoot = false;
        }
    }
}

void Scene_Play::changePlayerStateTo(const std::string &state) {
    auto &prev = m_player->getComponent<CState>().previousState;
    if (prev != state) {
        prev = m_player->getComponent<CState>().state;
        m_player->getComponent<CState>().state = state;
        m_player->getComponent<CState>().changeAnimation = true;
    } else {
        m_player->getComponent<CState>().changeAnimation = false;
    }
}

void Scene_Play::sAnimation() {
    // TODO: Complete the Animation class code first
    if (m_player->getComponent<CTransform>().velocity.y < 0) {
        if (!m_player->getComponent<CInput>().canShoot) {
            changePlayerStateTo("jump");
        } else {
            changePlayerStateTo("air_shoot");
        }
    } else {
        if (m_player->getComponent<CTransform>().velocity.x != 0) {
            if (!m_player->getComponent<CInput>().canShoot) {
                changePlayerStateTo("run_shoot");
            } else {
                changePlayerStateTo("run");
            }
        } else {
            if (!m_player->getComponent<CInput>().canShoot) {
                changePlayerStateTo("stand_shoot");
            } else {
                changePlayerStateTo("stand");
            }
        }
    }
    if (m_player->getComponent<CState>().changeAnimation) {
        std::string animationName{};
        if (m_player->getComponent<CState>().state == "run") {
            animationName = "Run";
        } else if (m_player->getComponent<CState>().state == "stand") {
            animationName = "Stand";
        } else if (m_player->getComponent<CState>().state == "jump") {
            animationName = "Jump";
        } else if (m_player->getComponent<CState>().state == "air_shoot") {
            animationName = "AirShoot";
        }  else if (m_player->getComponent<CState>().state == "run_shoot") {
            animationName = "RunShoot";
        }  else if (m_player->getComponent<CState>().state == "stand_shoot") {
            animationName = "StandShoot";
        }
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation(animationName), true);
    }

    for (const auto &entity: m_entityManager.getEntities()) {
        if (entity->getComponent<CAnimation>().animation.hasEnded() && !entity->getComponent<CAnimation>().repeat) {
            entity->destroy();
        }
        if (entity->hasComponent<CAnimation>()) {
            entity->getComponent<CAnimation>().animation.update();
        }
    }

    // if (m_player->getComponent<CState>().changeAnimation) {
    //     std::string animationName;
    //     if (m_player->getComponent<CState>().state == "stand") {
    //         animationName = "Stand";
    //     } else if (m_player->getComponent<CState>().state == "air") {
    //         animationName = "Jump";
    //     } else if (m_player->getComponent<CState>().state == "run") {
    //         animationName = "Run";
    //     } else if (m_player->getComponent<CState>().state == "standshoot") {
    //         animationName = "StandShoot";
    //     } else if (m_player->getComponent<CState>().state == "airshoot") {
    //         animationName = "AirShoot";
    //     } else if (m_player->getComponent<CState>().state == "runshoot") {
    //         animationName = "RunShoot";
    //     }
    //     // std::cout << "Ivan: getAnimation " << animationName << "\n";
    //     m_player->addComponent<CAnimation>(m_game->assets().getAnimation(animationName), true);
    // }
}

void Scene_Play::onEnd() {
    // TODO: when the scene ends, change back to the MENU scene
    // use m_game->changeScene(correct params);
    // m_game->changeScene( "MENU", std::make_shared<Scene_Menu>(m_game));
}

void Scene_Play::sRender() {
    // std::cout << "Entities size" << m_entityManager.getEntities().size() << std::endl;

    // color the background darker, so you know that the game is paused
    if (!m_paused) {
        m_game->window().clear(sf::Color(100, 100, 255));
    } else {
        m_game->window().clear(sf::Color(50, 50, 150));
    }

    // set the viewport of the window to be centered on the player if it's far enough right
    auto &pPos = m_player->getComponent<CTransform>().pos;
    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
    sf::View view = m_game->window().getView();
    view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
    m_game->window().setView(view);

    // draw all Entity textures / animations
    if (m_drawTextures) {
        for (const auto &e: m_entityManager.getEntities()) {
            auto &transform = e->getComponent<CTransform>();
            if (e->hasComponent<CAnimation>()) {
                auto &animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(
                    transform.pos.x, transform.pos.y
                );
                animation.getSprite().setScale(
                    transform.scale.x, transform.scale.y
                );
                m_game->window().draw(animation.getSprite());
            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangle shape
    if (m_drawCollision) {
        for (const auto &e: m_entityManager.getEntities()) {
            if (e->hasComponent<CBoundingBox>()) {
                auto &box = e->getComponent<CBoundingBox>();
                auto &transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color::White);
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }

    // draw the grid so that can easily debug
    if (m_drawGrid) {
        float leftX = m_game->window().getView().getCenter().x - width() / 2.0;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - ((int) leftX % (int) m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += m_gridSize.x) {
            drawLine(vec2(x, 0), vec2(x, height()));
        }

        for (float y = 0; y < height(); y += m_gridSize.y) {
            drawLine(vec2(leftX, height() - y), vec2(rightX, height() - y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x) {
                std::string xCell = std::to_string((int) x / (int) m_gridSize.x);
                std::string yCell = std::to_string((int) y / (int) m_gridSize.y);
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
                m_game->window().draw(m_gridText);
            }
        }
    }
}
