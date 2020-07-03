#include "Window.h"

Window::Window()
{
    Setup("Window", sf::Vector2u(640, 480));
}

Window::Window(const std::string& title, const sf::Vector2u& size)
{
    Setup(title, size);
}

Window::~Window()
{
    window_.close();
}

void Window::Setup(const std::string& title, const sf::Vector2u& size)
{
    window_title_ = title;
    window_size_ = size;
    is_fullscreen_ = false;
    is_done_ = false;
    is_focused_ = true;

    event_mgr_.AddCallback(StateType(0), "Fullscreen_toggle", &Window::ToggleFullscreen, this);
    event_mgr_.AddCallback(StateType(0), "Window_close", &Window::Close, this);

    Create();
}

void Window::Create()
{
    sf::Uint32 style = sf::Style::Default;
    if (is_fullscreen_) { style = sf::Style::Fullscreen; }

    window_.create(sf::VideoMode(window_size_.x, window_size_.y, 32), window_title_, style);
}

void Window::BeginDraw()
{
    window_.clear(sf::Color::Black);
}

void Window::EndDraw()
{
    window_.display();
}

bool Window::IsDone() const
{
    return is_done_;
}

bool Window::IsFullscreen() const
{
    return is_fullscreen_;
}

bool Window::IsFocused() const
{
    return is_focused_;
}

sf::RenderWindow* Window::GetRenderWindow()
{
    return &window_;
}

EventManager* Window::GetEventManager()
{
    return &event_mgr_;
}

sf::Vector2u Window::GetWindowSize() const
{
    return window_size_;
}

sf::FloatRect Window::GetViewSpace() const
{
    const sf::Vector2f viewCenter(window_.getView().getCenter());
    const sf::Vector2f viewSize(window_.getView().getSize());
    const sf::Vector2f viewSizeHalf(viewSize.x / 2, viewSize.y / 2);
    const sf::FloatRect viewSpace(viewCenter - viewSizeHalf, viewSize);
    return viewSpace;
}

void Window::ToggleFullscreen(EventDetails* details)
{
    is_fullscreen_ = !is_fullscreen_;
    window_.close();
    Create();
}

void Window::Close(EventDetails* details)
{
    is_done_ = true;
}

void Window::Update()
{
    sf::Event event;

    while (window_.pollEvent(event))
    {
        if (event.type == sf::Event::LostFocus)
        {
            is_focused_ = false;
            event_mgr_.SetFocus(false);
        }
        else if (event.type == sf::Event::GainedFocus)
        {
            is_focused_ = true;
            event_mgr_.SetFocus(true);
        }
        event_mgr_.HandleEvent(event);
    }

    event_mgr_.Update();
}
