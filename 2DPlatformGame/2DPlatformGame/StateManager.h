#pragma once

#include <vector>
#include <unordered_map>
#include "State_Intro.h"
#include "State_MainMenu.h"
#include "State_Game.h"
#include "State_Paused.h"
#include "State_GameOver.h"
#include "SharedContext.h"

enum class StateType
{
    Intro = 1,
    MainMenu,
    Game,
    Paused,
    GameOver,
    Credits
};

// State container.
using StateContainer = std::vector<std::pair<StateType, BaseState*>>;
// Type container.
using TypeContainer = std::vector<StateType>;
// State factory.
using StateFactory = std::unordered_map<StateType, std::function<BaseState*()>>;

class StateManager
{
public:
    StateManager(SharedContext* shared);
    ~StateManager();

    void Update(const sf::Time& time);
    void Draw();

    void ProcessRequests();

    SharedContext* GetContext() const;
    bool HasState(const StateType& type);

    void SwitchTo(const StateType& type);
    void Remove(const StateType& type);
private:

    // Methods.
    void CreateState(const StateType& type);
    void RemoveState(const StateType& type);

    template <class T>
    void RegisterState(const StateType& type)
    {
        state_factory_[type] = [this]() -> BaseState*
        {
            return new T(this);
        };
    }

    // Members.
    SharedContext* shared_context_;
    StateContainer states_;
    TypeContainer type_to_remove_;
    StateFactory state_factory_;
};
