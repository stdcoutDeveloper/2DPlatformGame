#pragma once

#include <string>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "EventManager.h"

class Window
{
public:
    Window();
    Window(const std::string& title, const sf::Vector2u& size);
    ~Window();

    void BeginDraw();
    void EndDraw();

    void Update();

    bool IsDone() const;
    bool IsFullscreen() const;
    bool IsFocused() const;

    void ToggleFullscreen(EventDetails* details);
    void Close(EventDetails* details = nullptr);

    sf::RenderWindow* GetRenderWindow();
    EventManager* GetEventManager();
    sf::Vector2u GetWindowSize() const;
    sf::FloatRect GetViewSpace() const;

private:
    void Setup(const std::string& title, const sf::Vector2u& size);
    void Create();

    sf::RenderWindow window_;
    EventManager event_mgr_;
    sf::Vector2u window_size_;
    std::string window_title_;
    bool is_done_;
    bool is_fullscreen_;
    bool is_focused_;
};
