#pragma once
#include "EntityBase.h"
#include "SpriteSheet.h"

class Character : public EntityBase
{
    friend class EntityManager;
public:
    Character(EntityManager* l_entityMgr);
    virtual ~Character();

    void Move(const Direction& l_dir);

    void Jump();
    void Attack();
    void GetHurt(const int& l_damage);

    void Load(const std::string& l_path);

    void OnEntityCollision(
        EntityBase* l_collider, bool l_attack) override = 0;

    void Update(float l_dT) override;
    void Draw(sf::RenderWindow* l_wind) override;
protected:
    void UpdateAttackAABB();
    void Animate();
    SpriteSheet m_spriteSheet;
    float m_jumpVelocity;

    int m_hitpoints;

    sf::FloatRect m_attackAABB;
    sf::Vector2f m_attackAABBoffset;
};
