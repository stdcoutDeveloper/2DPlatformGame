#pragma once

#include "Window.h"
#include "EventManager.h"
#include "StateManager.h"
#include "TextureManager.h"
#include "EntityManager.h"
#include <iostream>

class Game
{
public:
    Game();
    ~Game();

    void Update();
    void Render();
    void LateUpdate();

    sf::Time GetElapsed() const;

    Window* GetWindow();

private:
    void RestartClock();

    sf::Clock clock_;
    sf::Time elapsed_time_;
    SharedContext context_;
    Window window_;
    EntityManager entity_mgr_;
    TextureManager texture_mgr_;
    StateManager state_mgr_;
};
