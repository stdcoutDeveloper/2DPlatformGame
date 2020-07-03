#include "Player.h"
#include "EntityManager.h"
#include "StateManager.h"

Player::Player(EntityManager* entityMgr)
    : Character(entityMgr)
{
    Load("Player.char");
    type_ = EntityType::Player;

    EventManager* events = entity_mgr_->GetContext()->event_mgr_;
    events->AddCallback<Player>(StateType::Game, "Player_MoveLeft", &Player::React, this);
    events->AddCallback<Player>(StateType::Game, "Player_MoveRight", &Player::React, this);
    events->AddCallback<Player>(StateType::Game, "Player_Jump", &Player::React, this);
    events->AddCallback<Player>(StateType::Game, "Player_Attack", &Player::React, this);
}

Player::~Player()
{
    EventManager* events = entity_mgr_->GetContext()->event_mgr_;
    events->RemoveCallback(StateType::Game, "Player_MoveLeft");
    events->RemoveCallback(StateType::Game, "Player_MoveRight");
    events->RemoveCallback(StateType::Game, "Player_Jump");
    events->RemoveCallback(StateType::Game, "Player_Attack");
}

void Player::React(EventDetails* details)
{
    if (details->name_ == "Player_MoveLeft")
    {
        Move(Direction::Left);
    }
    else if (details->name_ == "Player_MoveRight")
    {
        Move(Direction::Right);
    }
    else if (details->name_ == "Player_Jump")
    {
        Jump();
    }
    else if (details->name_ == "Player_Attack")
    {
        Attack();
    }
}

void Player::OnEntityCollision(EntityBase* collider, bool isAttack)
{
    if (state_ == EntityState::Dying) { return; }
    if (isAttack)
    {
        if (state_ != EntityState::Attacking) { return; }
        if (!sprite_sheet_.GetCurrentAnim()->IsInAction()) { return; }
        if (collider->GetType() != EntityType::Enemy &&
            collider->GetType() != EntityType::Player)
        {
            return;
        }
        auto opponent = dynamic_cast<Character*>(collider);
        opponent->GetHurt(1);
        if (position_.x > opponent->GetPosition().x)
        {
            opponent->AddVelocity(-32, 0);
        }
        else
        {
            opponent->AddVelocity(32, 0);
        }
    }
    else
    {
        // Other behavior.
    }
}
