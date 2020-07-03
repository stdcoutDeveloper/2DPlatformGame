#include "State_Game.h"
#include "StateManager.h"

State_Game::State_Game(StateManager* stateManager)
    : BaseState(stateManager)
{
}

State_Game::~State_Game() = default;

void State_Game::OnCreate()
{
    EventManager* evMgr = state_mgr_->
                          GetContext()->event_mgr_;

    evMgr->AddCallback(StateType::Game, "Key_Escape", &State_Game::MainMenu, this);
    evMgr->AddCallback(StateType::Game, "Key_P", &State_Game::Pause, this);
    evMgr->AddCallback(StateType::Game, "Key_O", &State_Game::ToggleOverlay, this);

    const sf::Vector2u size = state_mgr_->GetContext()->wind_->GetWindowSize();
    view_.setSize(size.x * 1.0f, size.y * 1.0f);
    view_.setCenter(size.x / 2.0f, size.y / 2.0f);
    view_.zoom(0.6f);
    state_mgr_->GetContext()->wind_->GetRenderWindow()->setView(view_);

    game_map_ = new Map(state_mgr_->GetContext(), this);
    game_map_->LoadMap("media/Maps/map1.map");
}

void State_Game::OnDestroy()
{
    EventManager* evMgr = state_mgr_->
                          GetContext()->event_mgr_;
    evMgr->RemoveCallback(StateType::Game, "Key_Escape");
    evMgr->RemoveCallback(StateType::Game, "Key_P");
    evMgr->RemoveCallback(StateType::Game, "Key_O");

    delete game_map_;
    game_map_ = nullptr;
}

void State_Game::Update(const sf::Time& time)
{
    SharedContext* context = state_mgr_->GetContext();
    EntityBase* player = context->entity_mgr_->Find("Player");
    if (!player)
    {
        std::cout << "Respawning player..." << std::endl;
        context->entity_mgr_->Add(EntityType::Player, "Player");
        player = context->entity_mgr_->Find("Player");
        player->SetPosition(game_map_->GetPlayerStart());
    }
    else
    {
        view_.setCenter(player->GetPosition());
        context->wind_->GetRenderWindow()->setView(view_);
    }

    const sf::FloatRect viewSpace = context->wind_->GetViewSpace();
    if (viewSpace.left <= 0)
    {
        view_.setCenter(viewSpace.width / 2, view_.getCenter().y);
        context->wind_->GetRenderWindow()->setView(view_);
    }
    else if (viewSpace.left + viewSpace.width > (game_map_->GetMapSize().x + 1) * Tile_Size * 1.0f)
    {
        view_.setCenter(((game_map_->GetMapSize().x * 1.0f + 1) * Tile_Size) - (viewSpace.width / 2),
                        view_.getCenter().y);
        context->wind_->GetRenderWindow()->setView(view_);
    }

    game_map_->Update(time.asSeconds());
    state_mgr_->GetContext()->entity_mgr_->Update(time.asSeconds());
}

void State_Game::Draw()
{
    game_map_->Draw();
    state_mgr_->GetContext()->entity_mgr_->Draw();
}

void State_Game::MainMenu(EventDetails* details)
{
    state_mgr_->SwitchTo(StateType::MainMenu);
}

void State_Game::Pause(EventDetails* details)
{
    state_mgr_->SwitchTo(StateType::Paused);
}

void State_Game::Activate()
{
}

void State_Game::Deactivate()
{
}

// Test/debug methods.
void State_Game::ToggleOverlay(EventDetails* details)
{
    state_mgr_->GetContext()->debug_overlay_.SetDebug(!state_mgr_->GetContext()->debug_overlay_.Debug());
}
