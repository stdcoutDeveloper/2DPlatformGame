#include "Character.h"
#include "EntityManager.h"
#include "StateManager.h"

Character::Character(EntityManager* entityMgr)
    : EntityBase(entityMgr),
      sprite_sheet_(entity_mgr_->GetContext()->texture_mgr_),
      jump_velocity_(250), hit_points_(5)
{
    name_ = "Character";
}

Character::~Character() = default;

void Character::Move(const Direction& dir)
{
    if (GetState() == EntityState::Dying) { return; }
    sprite_sheet_.SetDirection(dir);
    if (dir == Direction::Left) { Accelerate(-speed_.x, 0); }
    else { Accelerate(speed_.x, 0); }
    if (GetState() == EntityState::Idle) { SetState(EntityState::Walking); }
}

void Character::Jump()
{
    if (GetState() == EntityState::Dying || GetState() == EntityState::Jumping || GetState() == EntityState::Hurt)
    {
        return;
    }
    SetState(EntityState::Jumping);
    AddVelocity(0, -jump_velocity_);
}

void Character::Attack()
{
    if (GetState() == EntityState::Dying || GetState() == EntityState::Jumping ||
        GetState() == EntityState::Hurt || GetState() == EntityState::Attacking)
    {
        return;
    }
    SetState(EntityState::Attacking);
}

void Character::GetHurt(const int& damage)
{
    if (GetState() == EntityState::Dying || GetState() == EntityState::Hurt) { return; }
    hit_points_ = (hit_points_ - damage > 0 ? hit_points_ - damage : 0);
    if (hit_points_) { SetState(EntityState::Hurt); }
    else { SetState(EntityState::Dying); }
}

void Character::Load(const std::string& path)
{
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + std::string("media/Characters/") + path);
    if (!file.is_open())
    {
        std::cout << "! Failed loading the character file: " << path << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line))
    {
        if (line[0] == '|') { continue; }
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "Name")
        {
            keystream >> name_;
        }
        else if (type == "Spritesheet")
        {
            std::string path;
            keystream >> path;
            sprite_sheet_.LoadSheet("media/SpriteSheets/" + path);
        }
        else if (type == "Hitpoints")
        {
            keystream >> hit_points_;
        }
        else if (type == "BoundingBox")
        {
            sf::Vector2f boundingSize;
            keystream >> boundingSize.x >> boundingSize.y;
            SetSize(boundingSize.x, boundingSize.y);
        }
        else if (type == "DamageBox")
        {
            keystream >> attack_box_offset_.x >> attack_box_offset_.y
                >> attack_box_.width >> attack_box_.height;
        }
        else if (type == "Speed")
        {
            keystream >> speed_.x >> speed_.y;
        }
        else if (type == "JumpVelocity")
        {
            keystream >> jump_velocity_;
        }
        else if (type == "MaxVelocity")
        {
            keystream >> max_velocity_.x >> max_velocity_.y;
        }
        else
        {
            std::cout << "! Unknown type in character file: " << type << std::endl;
        }
    }
    file.close();
}

void Character::UpdateAttackBox()
{
    attack_box_.left =
    (sprite_sheet_.GetDirection() == Direction::Left
         ? (bounding_box_.left - attack_box_.width) - attack_box_offset_.x
         : (bounding_box_.left + bounding_box_.width) + attack_box_offset_.x);
    attack_box_.top = bounding_box_.top + attack_box_offset_.y;
}

void Character::Animate()
{
    const EntityState state = GetState();

    if (state == EntityState::Walking && sprite_sheet_.
                                         GetCurrentAnim()->GetName() != "Walk")
    {
        sprite_sheet_.SetAnimation("Walk", true, true);
    }
    else if (state == EntityState::Jumping && sprite_sheet_.
                                              GetCurrentAnim()->GetName() != "Jump")
    {
        sprite_sheet_.SetAnimation("Jump", true, false);
    }
    else if (state == EntityState::Attacking && sprite_sheet_.
                                                GetCurrentAnim()->GetName() != "Attack")
    {
        sprite_sheet_.SetAnimation("Attack", true, false);
    }
    else if (state == EntityState::Hurt && sprite_sheet_.
                                           GetCurrentAnim()->GetName() != "Hurt")
    {
        sprite_sheet_.SetAnimation("Hurt", true, false);
    }
    else if (state == EntityState::Dying && sprite_sheet_.
                                            GetCurrentAnim()->GetName() != "Death")
    {
        sprite_sheet_.SetAnimation("Death", true, false);
    }
    else if (state == EntityState::Idle && sprite_sheet_.
                                           GetCurrentAnim()->GetName() != "Idle")
    {
        sprite_sheet_.SetAnimation("Idle", true, true);
    }
}

void Character::Update(float deltaTime)
{
    EntityBase::Update(deltaTime);
    if (attack_box_.width != 0 && attack_box_.height != 0)
    {
        UpdateAttackBox();

        // Debug.
        if (entity_mgr_->GetContext()->debug_overlay_.Debug())
        {
            auto arect = new sf::RectangleShape(sf::Vector2f(attack_box_.width, attack_box_.height));
            arect->setPosition(attack_box_.left, attack_box_.top);
            arect->setFillColor(sf::Color(255, 0, 0,
                                          (state_ == EntityState::Attacking && sprite_sheet_
                                                                               .GetCurrentAnim()->IsInAction()
                                               ? 200
                                               : 100)));
            entity_mgr_->GetContext()->debug_overlay_.Add(arect);
        }
        // End debug.
    }
    if (GetState() != EntityState::Dying && GetState() != EntityState::Attacking && GetState() != EntityState::Hurt)
    {
        if (abs(velocity_.y) >= 0.001f)
        {
            SetState(EntityState::Jumping);
        }
        else if (abs(velocity_.x) >= 0.1f)
        {
            SetState(EntityState::Walking);
        }
        else
        {
            SetState(EntityState::Idle);
        }
    }
    else if (GetState() == EntityState::Attacking || GetState() == EntityState::Hurt)
    {
        if (!sprite_sheet_.GetCurrentAnim()->IsPlaying())
        {
            SetState(EntityState::Idle);
        }
    }
    else if (GetState() == EntityState::Dying)
    {
        if (!sprite_sheet_.GetCurrentAnim()->IsPlaying())
        {
            entity_mgr_->Remove(id_);
        }
    }

    Animate();

    sprite_sheet_.Update(deltaTime);
    sprite_sheet_.SetSpritePosition(position_);
}

void Character::Draw(sf::RenderWindow* wind)
{
    sprite_sheet_.Draw(wind);
}
