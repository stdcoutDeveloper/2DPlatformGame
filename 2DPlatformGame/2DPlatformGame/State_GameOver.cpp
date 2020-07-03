#include "State_GameOver.h"
#include "StateManager.h"

State_GameOver::State_GameOver(StateManager* stateManager)
    : BaseState(stateManager)
{
}

State_GameOver::~State_GameOver() = default;

void State_GameOver::OnCreate()
{
    elapsed_time_ = 0;
    font_.loadFromFile(Utils::GetWorkingDirectory() + "media/Fonts/arial.ttf");
    text_.setFont(font_);
    text_.setCharacterSize(16);
    text_.setString("You beat the game! Congratulations!");
    /* 02/07/2020 Old code: setColor() is deprecated
     * Use function setFillColor() instead.
    m_text.setColor(sf::Color::White);
    */
    text_.setFillColor(sf::Color::White);
    text_.setOrigin(text_.getLocalBounds().width / 2,
                    text_.getLocalBounds().height / 2);
    text_.setPosition(400, 300);

    state_mgr_->Remove(StateType::Game);
}

void State_GameOver::OnDestroy()
{
}

void State_GameOver::Activate()
{
}

void State_GameOver::Deactivate()
{
}

void State_GameOver::Update(const sf::Time& time)
{
    elapsed_time_ += time.asSeconds();
    if (elapsed_time_ >= 5.0f)
    {
        state_mgr_->Remove(StateType::GameOver);
        state_mgr_->SwitchTo(StateType::MainMenu);
    }
}

void State_GameOver::Draw()
{
    sf::RenderWindow* window = state_mgr_->GetContext()->wind_->GetRenderWindow();
    window->draw(text_);
}
