#pragma once
#include "BaseState.h"
#include "EventManager.h"

class State_Intro : public BaseState
{
public:
    State_Intro(StateManager* l_stateManager);
    ~State_Intro();

    void OnCreate() override;
    void OnDestroy() override;

    void Activate() override;
    void Deactivate() override;

    void Update(const sf::Time& l_time) override;
    void Draw() override;

    void Continue(EventDetails* l_details);
private:
    sf::Sprite m_introSprite;
    sf::Font m_font;
    sf::Text m_text;
};
