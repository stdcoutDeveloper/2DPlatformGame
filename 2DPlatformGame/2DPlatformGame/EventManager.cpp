#include "EventManager.h"

EventManager::EventManager()
    : current_state_(StateType(0)), is_has_focus_(true)
{
    LoadBindings();
}

EventManager::~EventManager()
{
    for (auto& itr : bindings_)
    {
        delete itr.second;
    }
}

bool EventManager::AddBinding(Binding* binding)
{
    if (bindings_.find(binding->name_) != bindings_.end())
        return false;

    return bindings_.emplace(binding->name_, binding).second;
}

bool EventManager::RemoveBinding(const std::string& name)
{
    auto itr = bindings_.find(name);
    if (itr == bindings_.end()) { return false; }
    delete itr->second;
    bindings_.erase(itr);
    return true;
}

void EventManager::SetCurrentState(StateType state)
{
    current_state_ = state;
}

void EventManager::SetFocus(const bool& focus)
{
    is_has_focus_ = focus;
}

void EventManager::HandleEvent(sf::Event& event)
{
    // Handling SFML events.
    for (auto& b_itr : bindings_)
    {
        Binding* bind = b_itr.second;
        for (auto& e_itr : bind->events_)
        {
            const auto sfmlEvent = static_cast<EventType>(event.type);
            if (e_itr.first != sfmlEvent) { continue; }
            if (sfmlEvent == EventType::KeyDown || sfmlEvent == EventType::KeyUp)
            {
                if (e_itr.second.code_ == event.key.code)
                {
                    // Matching event/keystroke.
                    // Increase count.
                    if (bind->details_.key_code_ != -1)
                    {
                        bind->details_.key_code_ = e_itr.second.code_;
                    }
                    ++(bind->c);
                    break;
                }
            }
            else if (sfmlEvent == EventType::MButtonDown || sfmlEvent == EventType::MButtonUp)
            {
                if (e_itr.second.code_ == event.mouseButton.button)
                {
                    // Matching event/keystroke.
                    // Increase count.
                    bind->details_.mouse_.x = event.mouseButton.x;
                    bind->details_.mouse_.y = event.mouseButton.y;
                    if (bind->details_.key_code_ != -1)
                    {
                        bind->details_.key_code_ = e_itr.second.code_;
                    }
                    ++(bind->c);
                    break;
                }
            }
            else
            {
                // No need for additional checking.
                if (sfmlEvent == EventType::MouseWheel)
                {
                    bind->details_.mouse_wheel_delta_ = event.mouseWheel.delta;
                }
                else if (sfmlEvent == EventType::WindowResized)
                {
                    bind->details_.size_.x = event.size.width;
                    bind->details_.size_.y = event.size.height;
                }
                else if (sfmlEvent == EventType::TextEntered)
                {
                    bind->details_.text_entered_ = event.text.unicode;
                }
                ++(bind->c);
            }
        }
    }
}

void EventManager::Update()
{
    if (!is_has_focus_) { return; }
    for (auto& b_itr : bindings_)
    {
        Binding* bind = b_itr.second;
        for (auto& e_itr : bind->events_)
        {
            switch (e_itr.first)
            {
            case(EventType::Keyboard):
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(e_itr.second.code_)))
                {
                    if (bind->details_.key_code_ != -1)
                    {
                        bind->details_.key_code_ = e_itr.second.code_;
                    }
                    ++(bind->c);
                }
                break;
            case(EventType::Mouse):
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button(e_itr.second.code_)))
                {
                    if (bind->details_.key_code_ != -1)
                    {
                        bind->details_.key_code_ = e_itr.second.code_;
                    }
                    ++(bind->c);
                }
                break;
            case(EventType::Joystick):
                // Up for expansion.
                break;

            default:
                break;
            }
        }

        if (static_cast<int>(bind->events_.size()) == bind->c)
        {
            auto stateCallbacks = callbacks_.find(current_state_);
            auto otherCallbacks = callbacks_.find(StateType(0));

            if (stateCallbacks != callbacks_.end())
            {
                auto callItr = stateCallbacks->second.find(bind->name_);
                if (callItr != stateCallbacks->second.end())
                {
                    // Pass in information about events.
                    callItr->second(&bind->details_);
                }
            }

            if (otherCallbacks != callbacks_.end())
            {
                auto callItr = otherCallbacks->second.find(bind->name_);
                if (callItr != otherCallbacks->second.end())
                {
                    // Pass in information about events.
                    callItr->second(&bind->details_);
                }
            }
        }
        bind->c = 0;
        bind->details_.Clear();
    }
}

void EventManager::LoadBindings()
{
    std::string delimiter = ":";

    std::ifstream bindings;
    bindings.open(Utils::GetWorkingDirectory() + "keys.cfg");
    if (!bindings.is_open())
    {
        std::cout << "! Failed loading keys.cfg." << std::endl;
        return;
    }
    std::string line;
    while (std::getline(bindings, line))
    {
        std::stringstream keystream(line);
        std::string callbackName;
        keystream >> callbackName;
        auto bind = new Binding(callbackName);
        while (!keystream.eof())
        {
            std::string keyval;
            keystream >> keyval;
            int start = 0;
            int end = keyval.find(delimiter);
            if (static_cast<size_t>(end) == std::string::npos)
            {
                delete bind;
                bind = nullptr;
                break;
            }
            EventType type = EventType(stoi(keyval.substr(start, end - start)));
            int code = stoi(keyval.substr(end + delimiter.length(),
                                          keyval.find(delimiter, end + delimiter.length())));

            EventInfo eventInfo;
            eventInfo.code_ = code;
            bind->BindEvent(type, eventInfo);
        }

        if (!AddBinding(bind)) { delete bind; }
        bind = nullptr;
    }
    bindings.close();
}
