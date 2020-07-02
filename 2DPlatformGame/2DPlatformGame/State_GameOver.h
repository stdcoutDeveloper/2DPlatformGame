#pragma once
#include "BaseState.h"
#include <SFML/Graphics.hpp>

class State_GameOver : public BaseState
{
public:
    State_GameOver(StateManager* l_stateManager);
    ~State_GameOver();

    void OnCreate() override;
    void OnDestroy() override;

    void Activate() override;
    void Deactivate() override;

    void Update(const sf::Time& l_time) override;
    void Draw() override;
private:
    sf::Font m_font;
    sf::Text m_text;
    float m_elapsed;
};
