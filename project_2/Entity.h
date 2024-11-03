#ifndef ENTITY_H
#define ENTITY_H
#include "Components.h"
#include <chrono>


class Entity {
    friend class EntityManager;
    bool         m_active = true;
    size_t       m_id = 0;
    std::string  m_tag = "default";

    explicit Entity(size_t id, const std::string &tag = "default") : m_id(id), m_tag(tag) {}
public:

    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CShape> cShape;
    std::shared_ptr<CCollision> cCollision;
    std::shared_ptr<CInput> cInput;
    std::shared_ptr<CScore> cScore;
    std::shared_ptr<CLifespan> cLifespan;

    bool isActive() const;
    const std::string &tag() const;
    const size_t id() const;
    void destroy();
};

#endif //ENTITY_H
