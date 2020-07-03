#include "State_Intro.h"
#include "StateManager.h"

State_Intro::State_Intro(StateManager* stateManager)
    : BaseState(stateManager)
{
}

State_Intro::~State_Intro() = default;

void State_Intro::OnCreate()
{
    const sf::Vector2u windowSize = state_mgr_->GetContext()
                                              ->wind_->GetRenderWindow()->getSize();

    TextureManager* textureMgr = state_mgr_->GetContext()->texture_mgr_;
    textureMgr->RequireResource("Intro");
    intro_sprite_.setTexture(*textureMgr->GetResource("Intro"));
    intro_sprite_.setOrigin(textureMgr->GetResource("Intro")->getSize().x / 2.0f,
                            textureMgr->GetResource("Intro")->getSize().y / 2.0f);

    intro_sprite_.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

    font_.loadFromFile(Utils::GetWorkingDirectory() + "media/Fonts/arial.ttf");
    text_.setFont(font_);
    text_.setString(sf::String("Press SPACE to continue"));
    text_.setCharacterSize(15);
    const sf::FloatRect textRect = text_.getLocalBounds();
    text_.setOrigin(textRect.left + textRect.width / 2.0f,
                    textRect.top + textRect.height / 2.0f);
    text_.setPosition(intro_sprite_.getPosition().x,
                      intro_sprite_.getPosition().y + textureMgr->GetResource("Intro")->getSize().y / 1.5f);

    EventManager* evMgr = state_mgr_->
                          GetContext()->event_mgr_;
    evMgr->AddCallback(StateType::Intro, "Intro_Continue", &State_Intro::Continue, this);
}

void State_Intro::OnDestroy()
{
    TextureManager* textureMgr = state_mgr_->GetContext()->texture_mgr_;
    textureMgr->ReleaseResource("Intro");

    EventManager* evMgr = state_mgr_->
                          GetContext()->event_mgr_;
    evMgr->RemoveCallback(StateType::Intro, "Intro_Continue");
}

void State_Intro::Draw()
{
    sf::RenderWindow* window = state_mgr_->
                               GetContext()->wind_->GetRenderWindow();

    window->draw(intro_sprite_);
    window->draw(text_);
}

void State_Intro::Continue(EventDetails* details)
{
    state_mgr_->SwitchTo(StateType::MainMenu);
    state_mgr_->Remove(StateType::Intro);
}

void State_Intro::Update(const sf::Time& time)
{
}

void State_Intro::Activate()
{
}

void State_Intro::Deactivate()
{
}
