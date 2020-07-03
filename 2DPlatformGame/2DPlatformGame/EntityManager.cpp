#include "EntityManager.h"
#include "SharedContext.h"

EntityManager::EntityManager(SharedContext* context, unsigned int maxEntities)
    : context_(context), id_counter_(0), max_entities_(maxEntities)
{
    LoadEnemyTypes("EnemyList.list");
    RegisterEntity<Player>(EntityType::Player);
    RegisterEntity<Enemy>(EntityType::Enemy);
}

EntityManager::~EntityManager() { Purge(); }

int EntityManager::Add(const EntityType& type, const std::string& name)
{
    const auto itr = entity_factory_.find(type);
    if (itr == entity_factory_.end()) { return -1; }
    EntityBase* entity = itr->second();
    entity->id_ = id_counter_;
    if (!name.empty()) { entity->name_ = name; }

    entities_.emplace(id_counter_, entity);

    if (type == EntityType::Enemy)
    {
        auto enemyItr = enemy_types_.find(name);
        if (enemyItr != enemy_types_.end())
        {
            auto enemy = dynamic_cast<Enemy*>(entity);
            enemy->Load(enemyItr->second);
        }
    }

    ++id_counter_;
    return static_cast<int>(id_counter_ - 1);
}

EntityBase* EntityManager::Find(const std::string& name)
{
    for (auto& itr : entities_)
    {
        if (itr.second->GetName() == name)
        {
            return itr.second;
        }
    }
    return nullptr;
}

EntityBase* EntityManager::Find(unsigned int id)
{
    const auto itr = entities_.find(id);
    if (itr == entities_.end()) { return nullptr; }
    return itr->second;
}

void EntityManager::Remove(unsigned int id)
{
    entities_to_remove_.emplace_back(id);
}

void EntityManager::Update(float deltaTime)
{
    for (auto& itr : entities_)
    {
        itr.second->Update(deltaTime);
    }

    EntityCollisionCheck();
    ProcessRemovals();
}

void EntityManager::Draw()
{
    sf::RenderWindow* wnd = context_->wind_->GetRenderWindow();
    const sf::FloatRect viewSpace = context_->wind_->GetViewSpace();

    for (auto& itr : entities_)
    {
        if (!viewSpace.intersects(itr.second->bounding_box_)) { continue; }
        itr.second->Draw(wnd);
    }
}

void EntityManager::Purge()
{
    for (auto& itr : entities_)
    {
        delete itr.second;
    }
    entities_.clear();
    id_counter_ = 0;
}

void EntityManager::ProcessRemovals()
{
    while (entities_to_remove_.begin() != entities_to_remove_.end())
    {
        unsigned int id = entities_to_remove_.back();
        auto itr = entities_.find(id);
        if (itr != entities_.end())
        {
            std::cout << "Discarding entity: " << itr->second->GetId() << std::endl;
            delete itr->second;
            entities_.erase(itr);
        }
        entities_to_remove_.pop_back();
    }
}

void EntityManager::EntityCollisionCheck()
{
    if (entities_.empty()) { return; }
    for (auto itr = entities_.begin(); std::next(itr) != entities_.end(); ++itr)
    {
        for (auto itr2 = std::next(itr); itr2 != entities_.end(); ++itr2)
        {
            if (itr->first == itr2->first) { continue; }

            // Regular AABB bounding box collision.
            if (itr->second->bounding_box_.intersects(itr2->second->bounding_box_))
            {
                itr->second->OnEntityCollision(itr2->second, false);
                itr2->second->OnEntityCollision(itr->second, false);
            }

            const EntityType t1 = itr->second->GetType();
            const EntityType t2 = itr2->second->GetType();
            if (t1 == EntityType::Player || t1 == EntityType::Enemy)
            {
                auto c1 = dynamic_cast<Character*>(itr->second);
                if (c1->attack_box_.intersects(itr2->second->bounding_box_))
                {
                    c1->OnEntityCollision(itr2->second, true);
                }
            }

            if (t2 == EntityType::Player || t2 == EntityType::Enemy)
            {
                auto c2 = dynamic_cast<Character*>(itr2->second);
                if (c2->attack_box_.intersects(itr->second->bounding_box_))
                {
                    c2->OnEntityCollision(itr->second, true);
                }
            }
        }
    }
}

void EntityManager::LoadEnemyTypes(const std::string& name)
{
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + std::string("media/Characters/") + name);
    if (!file.is_open())
    {
        std::cout << "! Failed loading file: " << name << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line))
    {
        if (line[0] == '|') { continue; }
        std::stringstream keystream(line);
        std::string name;
        std::string charFile;
        keystream >> name >> charFile;
        enemy_types_.emplace(name, charFile);
    }
    file.close();
}

SharedContext* EntityManager::GetContext() const
{
    return context_;
}
