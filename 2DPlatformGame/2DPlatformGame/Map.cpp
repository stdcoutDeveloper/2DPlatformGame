#include "Map.h"
#include "StateManager.h"

Map::Map(SharedContext* context, BaseState* currentState)
    : default_tile_(context), max_map_size_(32, 32), tile_count_(0),
      tile_set_count_(0), map_gravity_(512.f), is_load_next_map_(false), current_state_(currentState),
      context_(context)
{
    context_->game_map_ = this;
    LoadTiles("tiles.cfg");
}

Map::~Map()
{
    PurgeMap();
    PurgeTileSet();
    context_->game_map_ = nullptr;
}

Tile* Map::GetTile(unsigned int x, unsigned int y)
{
    const auto itr = tile_map_.find(ConvertCoordinates(x, y));
    return (itr != tile_map_.end() ? itr->second : nullptr);
}

TileInfo* Map::GetDefaultTile() { return &default_tile_; }
float Map::GetGravity() const { return map_gravity_; }
unsigned int Map::GetTileSize() const { return Tile_Size; }
const sf::Vector2u& Map::GetMapSize() const { return max_map_size_; }
const sf::Vector2f& Map::GetPlayerStart() const { return player_start_; }

void Map::LoadMap(const std::string& path)
{
    std::ifstream mapFile;
    mapFile.open(Utils::GetWorkingDirectory() + path);
    if (!mapFile.is_open())
    {
        std::cout << "! Failed loading map file: " << path << std::endl;
        return;
    }
    EntityManager* entityMgr = context_->entity_mgr_;
    std::string line;
    std::cout << "--- Loading a map: " << path << std::endl;

    int playerId = -1;
    while (std::getline(mapFile, line))
    {
        if (line[0] == '|') { continue; }
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "TILE")
        {
            int tileId = 0;
            keystream >> tileId;
            if (tileId < 0)
            {
                std::cout << "! Bad tile id: " << tileId << std::endl;
                continue;
            }
            auto itr = tile_set_.find(tileId);
            if (itr == tile_set_.end())
            {
                std::cout << "! Tile id(" << tileId << ") was not found in tileset." << std::endl;
                continue;
            }
            sf::Vector2i tileCoords;
            keystream >> tileCoords.x >> tileCoords.y;
            if (static_cast<unsigned int>(tileCoords.x) > max_map_size_.x || static_cast<unsigned int>(tileCoords.y) >
                max_map_size_.y)
            {
                std::cout << "! Tile is out of range: " << tileCoords.x << " " << tileCoords.y << std::endl;
                continue;
            }
            Tile* tile = new Tile();
            // Bind properties of a tile from a set.
            tile->properties_ = itr->second;
            if (!tile_map_.emplace(ConvertCoordinates(tileCoords.x, tileCoords.y), tile).second)
            {
                // Duplicate tile detected!
                std::cout << "! Duplicate tile! : " << tileCoords.x
                    << "" << tileCoords.y << std::endl;
                delete tile;
                tile = nullptr;
                continue;
            }
            std::string warp;
            keystream >> warp;
            tile->is_warp_ = false;
            if (warp == "WARP") { tile->is_warp_ = true; }
        }
        else if (type == "BACKGROUND")
        {
            if (!background_texture_.empty()) { continue; }
            keystream >> background_texture_;
            if (!context_->texture_mgr_->RequireResource(background_texture_))
            {
                background_texture_ = "";
                continue;
            }
            sf::Texture* texture = context_->texture_mgr_->GetResource(background_texture_);
            background_.setTexture(*texture);
            sf::Vector2f viewSize = current_state_->GetView().getSize();
            sf::Vector2u textureSize = texture->getSize();
            sf::Vector2f scaleFactors;
            scaleFactors.x = viewSize.x / textureSize.x;
            scaleFactors.y = viewSize.y / textureSize.y;
            background_.setScale(scaleFactors);
        }
        else if (type == "SIZE")
        {
            keystream >> max_map_size_.x >> max_map_size_.y;
        }
        else if (type == "GRAVITY")
        {
            keystream >> map_gravity_;
        }
        else if (type == "DEFAULT_FRICTION")
        {
            keystream >> default_tile_.friction_.x >> default_tile_.friction_.y;
        }
        else if (type == "NEXTMAP")
        {
            keystream >> next_map_;
        }
        else if (type == "PLAYER")
        {
            if (playerId != -1) { continue; }
            // Set up the player position here.
            playerId = entityMgr->Add(EntityType::Player);
            if (playerId < 0) { continue; }
            float playerX = 0;
            float playerY = 0;
            keystream >> playerX >> playerY;
            entityMgr->Find(playerId)->SetPosition(playerX, playerY);
            player_start_ = sf::Vector2f(playerX, playerY);
        }
        else if (type == "ENEMY")
        {
            std::string enemyName;
            keystream >> enemyName;
            int enemyId = entityMgr->Add(EntityType::Enemy, enemyName);
            if (enemyId < 0) { continue; }
            float enemyX = 0;
            float enemyY = 0;
            keystream >> enemyX >> enemyY;
            entityMgr->Find(enemyId)->SetPosition(enemyX, enemyY);
        }
        else
        {
            // Something else.
            std::cout << "! Unknown type \"" << type << "\"." << std::endl;
        }
    }
    mapFile.close();
    std::cout << "--- Map Loaded! ---" << std::endl;
}

void Map::LoadNext()
{
    is_load_next_map_ = true;
}

void Map::LoadTiles(const std::string& path)
{
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + path);
    if (!file.is_open())
    {
        std::cout << "! Failed loading tile set file: " << path << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line))
    {
        if (line[0] == '|') { continue; }
        std::stringstream keystream(line);
        int tileId;
        keystream >> tileId;
        if (tileId < 0) { continue; }
        auto tile = new TileInfo(context_, "TileSheet", tileId);
        keystream >> tile->name_ >> tile->friction_.x >> tile->friction_.y >> tile->is_deadly_;
        if (!tile_set_.emplace(tileId, tile).second)
        {
            // Duplicate tile detected!
            std::cout << "! Duplicate tile type: " << tile->name_ << std::endl;
            delete tile;
        }
    }
    file.close();
}

void Map::Update(float deltaTime)
{
    if (is_load_next_map_)
    {
        PurgeMap();
        is_load_next_map_ = false;
        if (!next_map_.empty())
        {
            LoadMap("media/maps/" + next_map_);
        }
        else
        {
            current_state_->GetStateManager()->SwitchTo(StateType::GameOver);
        }
        next_map_ = "";
    }
    const sf::FloatRect viewSpace = context_->wind_->GetViewSpace();
    background_.setPosition(viewSpace.left, viewSpace.top);
}

void Map::Draw()
{
    sf::RenderWindow* l_wind = context_->wind_->GetRenderWindow();
    l_wind->draw(background_);
    const sf::FloatRect viewSpace = context_->wind_->GetViewSpace();

    const sf::Vector2i tileBegin(static_cast<int>(floor(viewSpace.left / Tile_Size)),
                                 static_cast<int>(floor(viewSpace.top / Tile_Size)));
    const sf::Vector2i tileEnd(static_cast<int>(ceil((viewSpace.left + viewSpace.width) / Tile_Size)),
                               static_cast<int>(ceil((viewSpace.top + viewSpace.height) / Tile_Size)));

    unsigned int count = 0;
    for (int x = tileBegin.x; x <= tileEnd.x; ++x)
    {
        for (int y = tileBegin.y; y <= tileEnd.y; ++y)
        {
            if (x < 0 || y < 0) { continue; }
            Tile* tile = GetTile(x, y);
            if (!tile) { continue; }
            sf::Sprite& sprite = tile->properties_->sprite_;
            sprite.setPosition(x * Tile_Size * 1.0f, y * Tile_Size * 1.0f);
            l_wind->draw(sprite);
            ++count;

            // Debug.
            if (context_->debug_overlay_.Debug())
            {
                if (tile->properties_->is_deadly_ || tile->is_warp_)
                {
                    auto tileMarker = new sf::RectangleShape(
                        sf::Vector2f(Tile_Size, Tile_Size));
                    tileMarker->setPosition(x * Tile_Size * 1.0f, y * Tile_Size * 1.0f);
                    if (tile->properties_->is_deadly_)
                    {
                        tileMarker->setFillColor(sf::Color(255, 0, 0, 100));
                    }
                    else if (tile->is_warp_)
                    {
                        tileMarker->setFillColor(sf::Color(0, 255, 0, 150));
                    }
                    context_->debug_overlay_.Add(tileMarker);
                }
            }
            // End debug.
        }
    }
}

unsigned int Map::ConvertCoordinates(unsigned int x, unsigned int y) const
{
    return (x * max_map_size_.x) + y; // Row-major.
}

void Map::PurgeMap()
{
    tile_count_ = 0;
    for (auto& itr : tile_map_)
    {
        delete itr.second;
    }
    tile_map_.clear();
    context_->entity_mgr_->Purge();

    if (background_texture_.empty()) { return; }
    context_->texture_mgr_->ReleaseResource(background_texture_);
    background_texture_ = "";
}

void Map::PurgeTileSet()
{
    for (auto& itr : tile_set_)
    {
        delete itr.second;
    }
    tile_set_.clear();
    tile_set_count_ = 0;
}
