#pragma once

#include <unordered_map>
#include <functional>
#include "Player.h"
#include "Enemy.h"

typedef std::unordered_map<unsigned int, EntityBase*> EntityContainer;
typedef std::unordered_map<EntityType, std::function<EntityBase*()>> EntityFactory;
typedef std::unordered_map<std::string, std::string> EnemyTypes;

struct SharedContext; // forward declaration.

class EntityManager
{
public:
    EntityManager(SharedContext* context, unsigned int maxEntities);
    ~EntityManager();

    int Add(const EntityType& type, const std::string& name = "");
    EntityBase* Find(unsigned int id);
    EntityBase* Find(const std::string& name);
    void Remove(unsigned int id);

    void Update(float deltaTime);
    void Draw();

    void Purge();

    SharedContext* GetContext() const;

private:
    template <class T>
    void RegisterEntity(const EntityType& type)
    {
        entity_factory_[type] = [this]() -> EntityBase*
        {
            return new T(this);
        };
    }

    void ProcessRemovals();
    void LoadEnemyTypes(const std::string& name);
    void EntityCollisionCheck();

    EntityContainer entities_;
    EnemyTypes enemy_types_;
    EntityFactory entity_factory_;
    SharedContext* context_;
    unsigned int id_counter_;
    unsigned int max_entities_; // not used???

    std::vector<unsigned int> entities_to_remove_;
};
