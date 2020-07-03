#pragma once

#include "BaseState.h"
#include "EventManager.h"

class State_Paused : public BaseState
{
public:
    State_Paused(StateManager* stateManager);
    ~State_Paused();

    void OnCreate() override;
    void OnDestroy() override;

    void Activate() override;
    void Deactivate() override;

    void Update(const sf::Time& time) override;
    void Draw() override;

    void Unpause(EventDetails* details);

private:
    sf::Font font_;
    sf::Text text_;
    sf::RectangleShape rect_;
};
