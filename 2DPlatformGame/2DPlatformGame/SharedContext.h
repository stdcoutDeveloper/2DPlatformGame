#pragma once

#include "Window.h"
#include "EventManager.h"
#include "TextureManager.h"
#include "EntityManager.h"
#include "DebugOverlay.h"

class Map; // forward declaration

struct SharedContext
{
    SharedContext() :
        wind_(nullptr),
        event_mgr_(nullptr),
        texture_mgr_(nullptr),
        entity_mgr_(nullptr),
        game_map_(nullptr)
    {
    }

    Window* wind_;
    EventManager* event_mgr_;
    TextureManager* texture_mgr_;
    EntityManager* entity_mgr_;
    Map* game_map_;
    DebugOverlay debug_overlay_;
};
