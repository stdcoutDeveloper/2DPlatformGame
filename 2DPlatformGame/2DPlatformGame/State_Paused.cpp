#include "State_Paused.h"
#include "StateManager.h"

State_Paused::State_Paused(StateManager* stateManager)
    : BaseState(stateManager)
{
}

State_Paused::~State_Paused() = default;

void State_Paused::OnCreate()
{
    SetTransparent(true); // Set our transparency flag.
    font_.loadFromFile(Utils::GetWorkingDirectory() + "media/Fonts/arial.ttf");
    text_.setFont(font_);
    text_.setString(sf::String("PAUSED"));
    text_.setCharacterSize(14);
    text_.setStyle(sf::Text::Bold);

    const sf::Vector2u windowSize = state_mgr_->
                                    GetContext()->wind_->GetRenderWindow()->getSize();

    const sf::FloatRect textRect = text_.getLocalBounds();
    text_.setOrigin(textRect.left + textRect.width / 2.0f,
                    textRect.top + textRect.height / 2.0f);
    text_.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

    rect_.setSize(sf::Vector2f(windowSize));
    rect_.setPosition(0, 0);
    rect_.setFillColor(sf::Color(0, 0, 0, 150));

    EventManager* evMgr = state_mgr_->
                          GetContext()->event_mgr_;
    evMgr->AddCallback(StateType::Paused, "Key_P",
                       &State_Paused::Unpause, this);
}

void State_Paused::OnDestroy()
{
    EventManager* evMgr = state_mgr_->
                          GetContext()->event_mgr_;
    evMgr->RemoveCallback(StateType::Paused, "Key_P");
}

void State_Paused::Draw()
{
    sf::RenderWindow* wind = state_mgr_->
                             GetContext()->wind_->GetRenderWindow();
    wind->draw(rect_);
    wind->draw(text_);
}

void State_Paused::Unpause(EventDetails* details)
{
    state_mgr_->SwitchTo(StateType::Game);
}

void State_Paused::Activate()
{
}

void State_Paused::Deactivate()
{
}

void State_Paused::Update(const sf::Time& time)
{
}
