#pragma once

#include <string>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

enum class EntityType
{
    Base,
    Enemy,
    Player
};

enum class EntityState
{
    Idle,
    Walking,
    Jumping,
    Attacking,
    Hurt,
    Dying
};

struct TileInfo; // forward declaration

struct CollisionElement
{
    CollisionElement(float area, TileInfo* info, const sf::FloatRect& bounds)
        : area_(area), tile_(info), tile_bounds_(bounds)
    {
    }

    float area_;
    TileInfo* tile_;
    sf::FloatRect tile_bounds_;
};

typedef std::vector<CollisionElement> Collisions;
bool SortCollisions(const CollisionElement& first, const CollisionElement& second);

class EntityManager; // forward declaration

class EntityBase
{
private:
    friend class EntityManager;

public:
    EntityBase(EntityManager* entityMgr);
    virtual ~EntityBase();

    const sf::Vector2f& GetPosition() const;
    const sf::Vector2f& GetSize() const;
    EntityState GetState() const;
    std::string GetName() const;
    unsigned int GetId() const;
    EntityType GetType() const;

    void SetPosition(float x, float y);
    void SetPosition(const sf::Vector2f& pos);
    void SetSize(float x, float y);
    void SetState(const EntityState& state);

    void Move(float x, float y);
    void AddVelocity(float x, float y);
    void Accelerate(float x, float y);
    void SetAcceleration(float x, float y);
    void ApplyFriction(float x, float y);

    virtual void Update(float deltaTime);
    virtual void Draw(sf::RenderWindow* wind) = 0;

protected:
    // Methods.
    void UpdateBoundingBox();
    void CheckCollisions();
    void ResolveCollisions();

    // Method for what THIS entity does TO the collider entity.
    virtual void OnEntityCollision(EntityBase* collider, bool attack) = 0;

    // Data members.
    std::string name_;
    EntityType type_;
    unsigned int id_; // Entity id in the entity manager.
    sf::Vector2f position_; // Current position.
    sf::Vector2f old_position_; // Position before entity moved.
    sf::Vector2f velocity_; // Current velocity.
    sf::Vector2f max_velocity_; // Maximum velocity.
    sf::Vector2f speed_; // Value of acceleration.
    sf::Vector2f acceleration_; // Current acceleration.
    sf::Vector2f friction_; // Default friction value.
    TileInfo* reference_tile_; // Tile underneath entity.
    sf::Vector2f bounding_box_size_; // Size of the collision box.
    sf::FloatRect bounding_box_; // The bounding box for collisions.
    EntityState state_; // Current entity state.
    // Flags for remembering axis collisions.
    bool is_colliding_on_x_;
    bool is_colliding_on_y_;

    Collisions collisions_;
    EntityManager* entity_mgr_;
};
