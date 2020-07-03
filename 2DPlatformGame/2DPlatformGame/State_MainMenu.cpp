#include "State_MainMenu.h"
#include "StateManager.h"

State_MainMenu::State_MainMenu(StateManager* stateManager)
    : BaseState(stateManager)
{
}

State_MainMenu::~State_MainMenu() = default;

void State_MainMenu::OnCreate()
{
    font_.loadFromFile(Utils::GetWorkingDirectory() + "media/Fonts/arial.ttf");
    text_.setFont(font_);
    text_.setString(sf::String("MAIN MENU:"));
    text_.setCharacterSize(18);

    const sf::FloatRect textRect = text_.getLocalBounds();
    text_.setOrigin(textRect.left + textRect.width / 2.0f,
                    textRect.top + textRect.height / 2.0f);

    text_.setPosition(400, 100);

    button_size_ = sf::Vector2f(300.0f, 32.0f);
    button_pos_ = sf::Vector2f(400, 200);
    button_padding_ = 4; // 4px.

    std::string str[3];
    str[0] = "PLAY";
    str[1] = "CREDITS";
    str[2] = "EXIT";

    for (int i = 0; i < 3; ++i)
    {
        sf::Vector2f buttonPosition(
            button_pos_.x, button_pos_.y +
            (i * 1.0f * (button_size_.y + button_padding_ * 1.0f)));
        rects_[i].setSize(button_size_);
        rects_[i].setFillColor(sf::Color::Red);

        rects_[i].setOrigin(
            button_size_.x / 2.0f, button_size_.y / 2.0f);
        rects_[i].setPosition(buttonPosition);

        labels_[i].setFont(font_);
        labels_[i].setString(sf::String(str[i]));
        labels_[i].setCharacterSize(12);

        const sf::FloatRect rect = labels_[i].getLocalBounds();
        labels_[i].setOrigin(
            rect.left + rect.width / 2.0f,
            rect.top + rect.height / 2.0f);

        labels_[i].setPosition(buttonPosition);
    }

    EventManager* evMgr = state_mgr_->
                          GetContext()->event_mgr_;
    evMgr->AddCallback(StateType::MainMenu, "Mouse_Left", &State_MainMenu::MouseClick, this);
}

void State_MainMenu::OnDestroy()
{
    EventManager* evMgr = state_mgr_->
                          GetContext()->event_mgr_;
    evMgr->RemoveCallback(StateType::MainMenu, "Mouse_Left");
}

void State_MainMenu::Activate()
{
    if (state_mgr_->HasState(StateType::Game)
        && labels_[0].getString() != "RESUME")
    {
        labels_[0].setString(sf::String("RESUME"));
    }
    else
    {
        labels_[0].setString(sf::String("PLAY"));
    }

    const sf::FloatRect rect = labels_[0].getLocalBounds();
    labels_[0].setOrigin(rect.left + rect.width / 2.0f,
                         rect.top + rect.height / 2.0f);
}

void State_MainMenu::MouseClick(EventDetails* details)
{
    //SharedContext* context = m_stateMgr->GetContext();
    const sf::Vector2i mousePos = details->mouse_;

    const float halfX = button_size_.x / 2.0f;
    const float halfY = button_size_.y / 2.0f;
    for (int i = 0; i < 3; ++i)
    {
        if (mousePos.x >= rects_[i].getPosition().x - halfX &&
            mousePos.x <= rects_[i].getPosition().x + halfX &&
            mousePos.y >= rects_[i].getPosition().y - halfY &&
            mousePos.y <= rects_[i].getPosition().y + halfY)
        {
            if (i == 0)
            {
                state_mgr_->SwitchTo(StateType::Game);
            }
            else if (i == 1)
            {
                // Credits state.
            }
            else if (i == 2)
            {
                state_mgr_->GetContext()->wind_->Close();
            }
        }
    }
}

void State_MainMenu::Draw()
{
    sf::RenderWindow* window = state_mgr_->
                               GetContext()->wind_->GetRenderWindow();
    window->draw(text_);
    for (int i = 0; i < 3; ++i)
    {
        window->draw(rects_[i]);
        window->draw(labels_[i]);
    }
}

void State_MainMenu::Update(const sf::Time& time)
{
}

void State_MainMenu::Deactivate()
{
}
