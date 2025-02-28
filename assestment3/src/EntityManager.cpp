#include <algorithm>

#include "EntityManager.h"

EntityManager::EntityManager()
{
}

void EntityManager::update()
{
    for (auto &entity : m_entitiesToAdd)
    {
        m_entities.push_back(entity);
        m_entityMap[entity->m_tag].push_back(entity);
    }
    m_entitiesToAdd.clear();

    // remove dead entities from the vector of all entities
    removeDeadEntities(m_entities);

    // remove dead entities from each vector in the entity map
    // C++17 way of iterating through [key,value] pairs in a map
    for (auto &[tag, entityVec] : m_entityMap)
    {
        removeDeadEntities(entityVec);
    }
}

void EntityManager::removeDeadEntities(EntityVec &vec)
{
    vec.erase(
        std::remove_if(
            vec.begin(),
            vec.end(),
            [](std::shared_ptr<Entity> &x) -> bool
            {
                return !x->isActive();
            }),
        vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag)
{
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_entitiesToAdd.push_back(entity);
    return entity;
}

const EntityVec &EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec &EntityManager::getEntities(const std::string &tag)
{
    return m_entityMap[tag];
}