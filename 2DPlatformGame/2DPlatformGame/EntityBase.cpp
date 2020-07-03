#include "EntityBase.h"
#include "EntityManager.h"
#include "SharedContext.h"
#include "Map.h"

bool SortCollisions(const CollisionElement& first, const CollisionElement& second)
{
    return first.area_ > second.area_;
}

EntityBase::EntityBase(EntityManager* entityMgr)
    : name_("BaseEntity"), type_(EntityType::Base),
      id_(0), reference_tile_(nullptr), state_(EntityState::Idle),
      is_colliding_on_x_(false), is_colliding_on_y_(false), entity_mgr_(entityMgr)
{
}

EntityBase::~EntityBase() = default;

void EntityBase::SetPosition(float x, float y)
{
    position_ = sf::Vector2f(x, y);
    UpdateBoundingBox();
}

void EntityBase::SetPosition(const sf::Vector2f& pos)
{
    position_ = pos;
    UpdateBoundingBox();
}

void EntityBase::SetSize(float x, float y)
{
    bounding_box_size_ = sf::Vector2f(x, y);
    UpdateBoundingBox();
}

void EntityBase::SetAcceleration(float x, float y)
{
    acceleration_ = sf::Vector2f(x, y);
}

void EntityBase::SetState(const EntityState& state)
{
    if (state_ == EntityState::Dying) { return; }
    state_ = state;
}

const sf::Vector2f& EntityBase::GetSize() const
{
    return bounding_box_size_;
}

std::string EntityBase::GetName() const
{
    return name_;
}

EntityState EntityBase::GetState() const
{
    return state_;
}

unsigned int EntityBase::GetId() const
{
    return id_;
}

EntityType EntityBase::GetType() const
{
    return type_;
}

const sf::Vector2f& EntityBase::GetPosition() const
{
    return position_;
}

void EntityBase::Move(float x, float y)
{
    old_position_ = position_;
    position_ += sf::Vector2f(x, y);
    const sf::Vector2u mapSize = entity_mgr_->GetContext()->game_map_->GetMapSize();
    if (position_.x < 0)
    {
        position_.x = 0;
    }
    else if (position_.x > (mapSize.x + 1) * Tile_Size * 1.0f)
    {
        position_.x = (mapSize.x + 1) * Tile_Size * 1.0f;
    }

    if (position_.y < 0)
    {
        position_.y = 0;
    }
    else if (position_.y > (mapSize.y + 1) * Tile_Size * 1.0f)
    {
        position_.y = (mapSize.y + 1) * Tile_Size * 1.0f;
        SetState(EntityState::Dying);
    }

    UpdateBoundingBox();
}

void EntityBase::AddVelocity(float x, float y)
{
    velocity_ += sf::Vector2f(x, y);
    if (abs(velocity_.x) > max_velocity_.x)
    {
        if (velocity_.x < 0) { velocity_.x = -max_velocity_.x; }
        else { velocity_.x = max_velocity_.x; }
    }

    if (abs(velocity_.y) > max_velocity_.y)
    {
        if (velocity_.y < 0) { velocity_.y = -max_velocity_.y; }
        else { velocity_.y = max_velocity_.y; }
    }
}

void EntityBase::Accelerate(float x, float y)
{
    acceleration_ += sf::Vector2f(x, y);
}

void EntityBase::ApplyFriction(float x, float y)
{
    if (velocity_.x != 0)
    {
        if (abs(velocity_.x) - abs(x) < 0) { velocity_.x = 0; }
        else
        {
            if (velocity_.x < 0) { velocity_.x += x; }
            else { velocity_.x -= x; }
        }
    }

    if (velocity_.y != 0)
    {
        if (abs(velocity_.y) - abs(y) < 0) { velocity_.y = 0; }
        else
        {
            if (velocity_.y < 0) { velocity_.y += y; }
            else { velocity_.y -= y; }
        }
    }
}

void EntityBase::Update(float deltaTime)
{
    Map* map = entity_mgr_->GetContext()->game_map_;
    const float gravity = map->GetGravity();
    Accelerate(0, gravity);
    AddVelocity(acceleration_.x * deltaTime, acceleration_.y * deltaTime);
    SetAcceleration(0.0f, 0.0f);
    sf::Vector2f frictionValue;
    if (reference_tile_)
    {
        frictionValue = reference_tile_->friction_;
        if (reference_tile_->is_deadly_) { SetState(EntityState::Dying); }
    }
    else if (map->GetDefaultTile())
    {
        frictionValue = map->GetDefaultTile()->friction_;
    }
    else
    {
        frictionValue = friction_;
    }

    const float friction_x = (speed_.x * frictionValue.x) * deltaTime;
    const float friction_y = (speed_.y * frictionValue.y) * deltaTime;
    ApplyFriction(friction_x, friction_y);
    const sf::Vector2f deltaPos = velocity_ * deltaTime;
    Move(deltaPos.x, deltaPos.y);
    is_colliding_on_x_ = false;
    is_colliding_on_y_ = false;
    CheckCollisions();
    ResolveCollisions();
}

void EntityBase::UpdateBoundingBox()
{
    bounding_box_ = sf::FloatRect(position_.x - (bounding_box_size_.x / 2), position_.y - bounding_box_size_.y,
                                  bounding_box_size_.x, bounding_box_size_.y);
}

void EntityBase::CheckCollisions()
{
    Map* gameMap = entity_mgr_->GetContext()->game_map_;
    const unsigned int tileSize = gameMap->GetTileSize();
    const int fromX = static_cast<int>(floor(bounding_box_.left / tileSize));
    const int toX = static_cast<int>(floor((bounding_box_.left + bounding_box_.width) / tileSize));
    const int fromY = static_cast<int>(floor(bounding_box_.top / tileSize));
    const int toY = static_cast<int>(floor((bounding_box_.top + bounding_box_.height) / tileSize));

    for (int x = fromX; x <= toX; ++x)
    {
        for (int y = fromY; y <= toY; ++y)
        {
            Tile* tile = gameMap->GetTile(x, y);
            if (!tile) { continue; }
            sf::FloatRect tileBounds(x * tileSize * 1.0f, y * tileSize * 1.0f, tileSize * 1.0f, tileSize * 1.0f);
            sf::FloatRect intersection;
            bounding_box_.intersects(tileBounds, intersection);
            const float area = intersection.width * intersection.height;

            CollisionElement e(area, tile->properties_, tileBounds);
            collisions_.emplace_back(e);
            if (tile->is_warp_ && type_ == EntityType::Player)
            {
                gameMap->LoadNext();
            }
        }
    }
}

void EntityBase::ResolveCollisions()
{
    if (!collisions_.empty())
    {
        std::sort(collisions_.begin(), collisions_.end(), SortCollisions);
        Map* gameMap = entity_mgr_->GetContext()->game_map_;
        const unsigned int tileSize = gameMap->GetTileSize();
        for (auto& itr : collisions_)
        {
            if (!bounding_box_.intersects(itr.tile_bounds_)) { continue; }
            // Debug
            if (entity_mgr_->GetContext()->debug_overlay_.Debug())
            {
                sf::Vector2f tempPos(itr.tile_bounds_.left, itr.tile_bounds_.top);
                auto rect = new sf::RectangleShape(sf::Vector2f(tileSize * 1.0f, tileSize * 1.0f));
                rect->setPosition(tempPos);
                rect->setFillColor(sf::Color(255, 255, 0, 150));
                entity_mgr_->GetContext()->debug_overlay_.Add(rect);
            }
            // End debug.
            const float xDiff = (bounding_box_.left + (bounding_box_.width / 2)) - (itr.tile_bounds_.left + (itr
                                                                                                             .tile_bounds_
                                                                                                             .width /
                2));
            const float yDiff = (bounding_box_.top + (bounding_box_.height / 2)) - (itr.tile_bounds_.top + (itr
                                                                                                            .tile_bounds_
                                                                                                            .height /
                2));
            float resolve = 0.0f;
            if (abs(xDiff) > abs(yDiff))
            {
                if (xDiff > 0)
                {
                    resolve = (itr.tile_bounds_.left + tileSize * 1.0f) - bounding_box_.left;
                }
                else
                {
                    resolve = -((bounding_box_.left + bounding_box_.width) - itr.tile_bounds_.left);
                }
                Move(resolve, 0);
                velocity_.x = 0;
                is_colliding_on_x_ = true;
            }
            else
            {
                if (yDiff > 0)
                {
                    resolve = (itr.tile_bounds_.top + tileSize * 1.0f) - bounding_box_.top;
                }
                else
                {
                    resolve = -((bounding_box_.top + bounding_box_.height) - itr.tile_bounds_.top);
                }
                Move(0, resolve);
                velocity_.y = 0;
                if (is_colliding_on_y_) { continue; }
                reference_tile_ = itr.tile_;
                is_colliding_on_y_ = true;
            }
        }
        collisions_.clear();
    }
    if (!is_colliding_on_y_) { reference_tile_ = nullptr; }
}
