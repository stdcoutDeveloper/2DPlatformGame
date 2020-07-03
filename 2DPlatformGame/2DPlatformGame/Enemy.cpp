#include "Enemy.h"

Enemy::Enemy(EntityManager* entityMgr)
    : Character(entityMgr), is_has_destination_(false)
{
    type_ = EntityType::Enemy;
}

Enemy::~Enemy() = default;

void Enemy::OnEntityCollision(EntityBase* collider, bool attack)
{
    if (state_ == EntityState::Dying) { return; }
    if (attack) { return; }
    if (collider->GetType() != EntityType::Player) { return; }
    auto player = dynamic_cast<Character*>(collider);
    SetState(EntityState::Attacking);
    player->GetHurt(1);
    if (position_.x > player->GetPosition().x)
    {
        player->AddVelocity(-speed_.x, 0);
        sprite_sheet_.SetDirection(Direction::Left);
    }
    else
    {
        player->AddVelocity(speed_.y, 0);
        sprite_sheet_.SetDirection(Direction::Right);
    }
}

void Enemy::Update(float deltaTime)
{
    Character::Update(deltaTime);

    if (is_has_destination_)
    {
        if (abs(destination_.x - position_.x) < 16)
        {
            is_has_destination_ = false;
            return;
        }
        if (destination_.x - position_.x > 0) { Move(Direction::Right); }
        else { Move(Direction::Left); }
        if (is_colliding_on_x_) { is_has_destination_ = false; }
        return;
    }
    const int random = rand() % 1000 + 1;
    if (random != 1000) { return; }
    int newX = rand() % 65 + 0;
    if (rand() % 2) { newX = -newX; }
    destination_.x = position_.x + newX * 1.0f;
    if (destination_.x < 0) { destination_.x = 0; }
    is_has_destination_ = true;
}
