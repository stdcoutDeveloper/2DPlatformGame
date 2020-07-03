#pragma once

#include "Character.h"

class Enemy : public Character
{
public:
    Enemy(EntityManager* entityMgr);
    ~Enemy();

    void OnEntityCollision(EntityBase* collider, bool attack) override;
    void Update(float deltaTime) override;

private:
    sf::Vector2f destination_;
    bool is_has_destination_;
};
