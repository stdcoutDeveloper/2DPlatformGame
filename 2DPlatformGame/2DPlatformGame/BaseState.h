#pragma once

#include <SFML/Graphics.hpp>

class StateManager; // forward declaration

class BaseState
{
private:
    friend class StateManager;

public:
    BaseState(StateManager* stateMgr)
        : state_mgr_(stateMgr), is_transparent_(false),
          is_transcendent_(false)
    {
    }

    virtual ~BaseState() = default;

    virtual void OnCreate() = 0;
    virtual void OnDestroy() = 0;

    virtual void Activate() = 0;
    virtual void Deactivate() = 0;

    virtual void Update(const sf::Time& time) = 0;
    virtual void Draw() = 0;

    void SetTransparent(const bool& transparent)
    {
        is_transparent_ = transparent;
    }

    bool IsTransparent() const
    {
        return is_transparent_;
    }

    void SetTranscendent(const bool& transcendence)
    {
        is_transcendent_ = transcendence;
    }

    bool IsTranscendent() const
    {
        return is_transcendent_;
    }

    sf::View& GetView()
    {
        return view_;
    }

    StateManager* GetStateManager() const
    {
        return state_mgr_;
    }

protected:
    StateManager* state_mgr_;
    bool is_transparent_;
    bool is_transcendent_;
    sf::View view_;
};
