#pragma once

#include "BaseState.h"
#include "EventManager.h"

class State_MainMenu : public BaseState
{
public:
    State_MainMenu(StateManager* stateManager);
    ~State_MainMenu();

    void OnCreate() override;
    void OnDestroy() override;

    void Activate() override;
    void Deactivate() override;

    void Update(const sf::Time& time) override;
    void Draw() override;

    void MouseClick(EventDetails* details);

private:
    sf::Font font_;
    sf::Text text_;

    sf::Vector2f button_size_;
    sf::Vector2f button_pos_;
    unsigned int button_padding_;

    sf::RectangleShape rects_[3];
    sf::Text labels_[3];
};
