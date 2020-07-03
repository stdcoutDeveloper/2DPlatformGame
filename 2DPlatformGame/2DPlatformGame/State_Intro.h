#pragma once

#include "BaseState.h"
#include "EventManager.h"

class State_Intro : public BaseState
{
public:
    State_Intro(StateManager* stateManager);
    ~State_Intro();

    void OnCreate() override;
    void OnDestroy() override;

    void Activate() override;
    void Deactivate() override;

    void Update(const sf::Time& time) override;
    void Draw() override;

    void Continue(EventDetails* details);

private:
    sf::Sprite intro_sprite_;
    sf::Font font_;
    sf::Text text_;
};
