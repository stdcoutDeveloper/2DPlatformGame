#pragma once

#include "BaseState.h"
#include "Map.h"
#include "EventManager.h"

class State_Game : public BaseState
{
public:
    State_Game(StateManager* stateManager);
    ~State_Game();

    void OnCreate() override;
    void OnDestroy() override;

    void Activate() override;
    void Deactivate() override;

    void Update(const sf::Time& time) override;
    void Draw() override;

    void MainMenu(EventDetails* details);
    void Pause(EventDetails* details);
    void ToggleOverlay(EventDetails* details);

private:
    Map* game_map_;
};
