#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "Entity.h"

using EntityVec = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::map<std::string, EntityVec>;

class EntityManager
{
private:
    EntityVec m_entities;       // All entities in the game
    EntityVec m_entitiesToAdd;  // Entities to be added in the next update
    EntityMap m_entityMap;      // Entities organized by tag
    size_t m_totalEntities = 0; // Total number of entities created

    void removeDeadEntities(EntityVec &vec); // Helper function to remove dead entities

public:
    EntityManager();

    void update(); // Updates the entity manager (adds new entities and removes dead ones)

    std::shared_ptr<Entity> addEntity(const std::string &tag); // Adds a new entity with a given tag

    const EntityVec &getEntities();                       // Returns all entities
    const EntityVec &getEntities(const std::string &tag); // Returns all entities with a specific tag
};
