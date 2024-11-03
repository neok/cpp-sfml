
#include "EntityManager.h"
#include "Entity.h"

EntityManager::EntityManager() {

}

void EntityManager::update() {

    for (auto &e: m_entitiesToAdd) {
        m_entities.push_back(e);
    }
    m_entitiesToAdd.clear();
    removeDeadEntities(m_entities);


    for (auto& [tag, entityVector] : m_entityMap) {
      removeDeadEntities(entityVector);
    }

}

void EntityManager::removeDeadEntities(EntityVector &vec) {
    vec.erase(
            std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity>& entity) {
                return !entity->isActive(); // If the entity is not active, it will be removed
            }),
            vec.end()
        );
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag) {
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_entitiesToAdd.push_back(entity);
    m_entityMap[tag].push_back(entity);

    return entity;
}

const EntityVector &EntityManager::getEntities() {
    return m_entities;
}

const EntityVector &EntityManager::getEntities(const std::string &tag) {
    auto it = m_entityMap.find(tag);

    if (it != m_entityMap.end()) {
        return it->second;
    }

    static EntityVector emptyVector; // Static so we can return a reference
    return emptyVector;
}

