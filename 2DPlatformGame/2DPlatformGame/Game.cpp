#include "Game.h"

Game::Game() : window_("Chapter 7", sf::Vector2u(800, 600)),
               entity_mgr_(&context_, 100), state_mgr_(&context_)
{
    clock_.restart();
    srand(static_cast<unsigned int>(time(nullptr)));

    context_.wind_ = &window_;
    context_.event_mgr_ = window_.GetEventManager();
    context_.texture_mgr_ = &texture_mgr_;
    context_.entity_mgr_ = &entity_mgr_;

    state_mgr_.SwitchTo(StateType::Intro);
}

Game::~Game() = default;

sf::Time Game::GetElapsed() const
{
    return clock_.getElapsedTime();
}

void Game::RestartClock()
{
    elapsed_time_ = clock_.restart();
}

Window* Game::GetWindow()
{
    return &window_;
}

void Game::Update()
{
    window_.Update();
    state_mgr_.Update(elapsed_time_);
}

void Game::Render()
{
    window_.BeginDraw();
    // Render here.
    state_mgr_.Draw();

    // Debug.
    if (context_.debug_overlay_.Debug())
    {
        context_.debug_overlay_.Draw(window_.GetRenderWindow());
    }
    // End debug.

    window_.EndDraw();
}

void Game::LateUpdate()
{
    state_mgr_.ProcessRequests();
    RestartClock();
}
