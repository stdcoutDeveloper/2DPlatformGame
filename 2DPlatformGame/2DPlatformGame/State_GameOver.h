#pragma once

#include "BaseState.h"
#include <SFML/Graphics.hpp>

class State_GameOver : public BaseState
{
public:
    State_GameOver(StateManager* stateManager);
    ~State_GameOver();

    void OnCreate() override;
    void OnDestroy() override;

    void Activate() override;
    void Deactivate() override;

    void Update(const sf::Time& time) override;
    void Draw() override;

private:
    sf::Font font_;
    sf::Text text_;
    float elapsed_time_;
};
