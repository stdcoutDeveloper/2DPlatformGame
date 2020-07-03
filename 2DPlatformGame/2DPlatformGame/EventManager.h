#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Utilities.h"

enum class EventType
{
    KeyDown = sf::Event::KeyPressed,
    KeyUp = sf::Event::KeyReleased,
    MButtonDown = sf::Event::MouseButtonPressed,
    MButtonUp = sf::Event::MouseButtonReleased,
    MouseWheel = sf::Event::MouseWheelMoved,
    WindowResized = sf::Event::Resized,
    GainedFocus = sf::Event::GainedFocus,
    LostFocus = sf::Event::LostFocus,
    MouseEntered = sf::Event::MouseEntered,
    MouseLeft = sf::Event::MouseLeft,
    Closed = sf::Event::Closed,
    TextEntered = sf::Event::TextEntered,
    Keyboard = sf::Event::Count + 1,
    Mouse,
    Joystick
};

struct EventInfo
{
    EventInfo()
    {
        code_ = 0;
    }

    EventInfo(int l_event)
    {
        code_ = l_event;
    }

    union
    {
        int code_;
    };
};

struct EventDetails
{
    EventDetails(const std::string& bindName)
        : name_(bindName)
    {
        Clear();
    }

    std::string name_;

    sf::Vector2i size_;
    sf::Uint32 text_entered_;
    sf::Vector2i mouse_;
    int mouse_wheel_delta_;
    int key_code_; // Single key code.

    void Clear()
    {
        size_ = sf::Vector2i(0, 0);
        text_entered_ = 0;
        mouse_ = sf::Vector2i(0, 0);
        mouse_wheel_delta_ = 0;
        key_code_ = -1;
    }
};

using Events = std::vector<std::pair<EventType, EventInfo>>;

struct Binding
{
    Binding(const std::string& name) : name_(name), c(0), details_(name)
    {
    }

    ~Binding() = default;

    void BindEvent(EventType type, EventInfo info = EventInfo())
    {
        events_.emplace_back(type, info);
    }

    Events events_;
    std::string name_;
    int c; // Count of events that are "happening".

    EventDetails details_;
};

using Bindings = std::unordered_map<std::string, Binding*>;
// Callback container.
using CallbackContainer = std::unordered_map<std::string, std::function<void(EventDetails*)>>;
// State callback container.

enum class StateType;

using Callbacks = std::unordered_map<StateType, CallbackContainer>;

class EventManager
{
public:
    EventManager();
    ~EventManager();

    bool AddBinding(Binding* binding);
    bool RemoveBinding(const std::string& name);

    void SetCurrentState(StateType state);
    void SetFocus(const bool& focus);

    // Needs to be defined in the header!
    template <class T>
    bool AddCallback(StateType state, const std::string& name,
                     void (T::*func)(EventDetails*), T* instance)
    {
        auto itr = callbacks_.emplace(state, CallbackContainer()).first;
        auto temp = std::bind(func, instance, std::placeholders::_1);
        return itr->second.emplace(name, temp).second;
    }

    bool RemoveCallback(StateType state, const std::string& name)
    {
        auto itr = callbacks_.find(state);
        if (itr == callbacks_.end()) { return false; }
        const auto itr2 = itr->second.find(name);
        if (itr2 == itr->second.end()) { return false; }
        itr->second.erase(name);
        return true;
    }

    void HandleEvent(sf::Event& event);
    void Update();

    // Getters.
    sf::Vector2i GetMousePos(sf::RenderWindow* wind = nullptr) const
    {
        return (wind ? sf::Mouse::getPosition(*wind) : sf::Mouse::getPosition());
    }

private:
    void LoadBindings();

    StateType current_state_;
    Bindings bindings_;
    Callbacks callbacks_;

    bool is_has_focus_;
};
