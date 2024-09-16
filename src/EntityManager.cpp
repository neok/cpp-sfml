
#include "EntityManager.h"
#include "Entity.h"

EntityManager::EntityManager() {

}

void EntityManager::update() {
    removeDeadEntities(m_entities);


    for (auto& [tag, entityVector] : m_entityMap) {
      removeDeadEntities(entityVector);
      }

}

void EntityManager::removeDeadEntities(EntityVector &vec) {
    //todo impl
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag) {
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_entitiesToAdd.push_back(entity);

    return entity;
}

const EntityVector &EntityManager::getEntities() {
    return m_entities;
}

const EntityVector &EntityManager::getEntities(const std::string &tag) {
    return m_entities;
}
