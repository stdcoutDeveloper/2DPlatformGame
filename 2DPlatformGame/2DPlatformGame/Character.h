#pragma once

#include "EntityBase.h"
#include "SpriteSheet.h"

class Character : public EntityBase
{
private:
    friend class EntityManager;

public:
    Character(EntityManager* entityMgr);
    virtual ~Character();

    void Move(const Direction& dir);

    void Jump();
    void Attack();
    void GetHurt(const int& damage);

    void Load(const std::string& path);

    void OnEntityCollision(
        EntityBase* collider, bool attack) override = 0;

    void Update(float deltaTime) override;
    void Draw(sf::RenderWindow* wind) override;

protected:
    SpriteSheet sprite_sheet_;
    float jump_velocity_;

    int hit_points_;

    sf::FloatRect attack_box_;
    sf::Vector2f attack_box_offset_;

    void UpdateAttackBox();
    void Animate();
};
