#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <map>
#include <array>
#include <fstream>
#include <sstream>
#include "Utilities.h"
#include "SharedContext.h"
#include "BaseState.h"

enum Sheet
{
    Tile_Size = 32,
    Sheet_Width = 256,
    Sheet_Height = 256
};

using TileID = unsigned int;

struct TileInfo
{
    TileInfo(SharedContext* context,
             const std::string& texture = "", TileID id = 0)
        : id_(0), is_deadly_(false), context_(context)
    {
        TextureManager* tmgr = context->texture_mgr_;
        if (texture.empty())
        {
            id_ = id;
            return;
        }
        if (!tmgr->RequireResource(texture)) { return; }
        texture_ = texture;
        id_ = id;
        sprite_.setTexture(*tmgr->GetResource(texture_));
        const sf::IntRect tileBoundaries(id_ % (Sheet_Width / Tile_Size) * Tile_Size,
                                         id_ / (Sheet_Height / Tile_Size) * Tile_Size,
                                         Tile_Size, Tile_Size);
        sprite_.setTextureRect(tileBoundaries);
    }

    ~TileInfo()
    {
        if (texture_.empty()) { return; }
        context_->texture_mgr_->ReleaseResource(texture_);
    }

    sf::Sprite sprite_;

    TileID id_;
    std::string name_;
    sf::Vector2f friction_;
    bool is_deadly_;

    SharedContext* context_;
    std::string texture_;
};

struct Tile
{
    TileInfo* properties_;
    bool is_warp_; // Is the tile a warp.
    // Other flags unique to each tile.
};

using TileMap = std::unordered_map<TileID, Tile*>;
using TileSet = std::unordered_map<TileID, TileInfo*>;

class Map
{
public:
    Map(SharedContext* context, BaseState* currentState);
    ~Map();

    Tile* GetTile(unsigned int x, unsigned int y);
    TileInfo* GetDefaultTile();

    float GetGravity() const;
    unsigned int GetTileSize() const;
    const sf::Vector2u& GetMapSize() const;
    const sf::Vector2f& GetPlayerStart() const;

    void LoadMap(const std::string& path);
    void LoadNext();

    void Update(float deltaTime);
    void Draw();
private:
    // Method for converting 2D coordinates to 1D ints.
    unsigned int ConvertCoordinates(unsigned int x, unsigned int y) const;
    void LoadTiles(const std::string& path);
    void PurgeMap();
    void PurgeTileSet();

    TileSet tile_set_;
    TileMap tile_map_;
    sf::Sprite background_;
    TileInfo default_tile_;
    sf::Vector2u max_map_size_;
    sf::Vector2f player_start_;
    unsigned int tile_count_;
    unsigned int tile_set_count_;
    float map_gravity_;
    std::string next_map_;
    bool is_load_next_map_;
    std::string background_texture_;
    BaseState* current_state_;
    SharedContext* context_;
};
