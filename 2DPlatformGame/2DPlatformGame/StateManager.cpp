#include "StateManager.h"

StateManager::StateManager(SharedContext* shared)
    : shared_context_(shared)
{
    RegisterState<State_Intro>(StateType::Intro);
    RegisterState<State_MainMenu>(StateType::MainMenu);
    RegisterState<State_Game>(StateType::Game);
    RegisterState<State_Paused>(StateType::Paused);
    RegisterState<State_GameOver>(StateType::GameOver);
}

StateManager::~StateManager()
{
    for (auto& itr : states_)
    {
        itr.second->OnDestroy();
        delete itr.second;
    }
}

void StateManager::Update(const sf::Time& time)
{
    if (states_.empty()) { return; }
    if (states_.back().second->IsTranscendent() && states_.size() > 1)
    {
        auto itr = states_.end();
        while (itr != states_.begin())
        {
            if (itr != states_.end())
            {
                if (!itr->second->IsTranscendent())
                {
                    break;
                }
            }
            --itr;
        }
        for (; itr != states_.end(); ++itr)
        {
            itr->second->Update(time);
        }
    }
    else
    {
        states_.back().second->Update(time);
    }
}

void StateManager::Draw()
{
    if (states_.empty()) { return; }
    if (states_.back().second->IsTransparent() && states_.size() > 1)
    {
        auto itr = states_.end();
        while (itr != states_.begin())
        {
            if (itr != states_.end())
            {
                if (!itr->second->IsTransparent())
                {
                    break;
                }
            }
            --itr;
        }
        for (; itr != states_.end(); ++itr)
        {
            shared_context_->wind_->GetRenderWindow()->setView(itr->second->GetView());
            itr->second->Draw();
        }
    }
    else
    {
        states_.back().second->Draw();
    }
}

SharedContext* StateManager::GetContext() const
{
    return shared_context_;
}

bool StateManager::HasState(const StateType& type)
{
    for (auto itr : states_)
    {
        if (itr.first == type)
        {
            const auto removed = std::find(type_to_remove_.begin(), type_to_remove_.end(), type);
            if (removed == type_to_remove_.end()) { return true; }
            return false;
        }
    }
    return false;
}

void StateManager::ProcessRequests()
{
    while (type_to_remove_.begin() != type_to_remove_.end())
    {
        RemoveState(*type_to_remove_.begin());
        type_to_remove_.erase(type_to_remove_.begin());
    }
}

void StateManager::SwitchTo(const StateType& type)
{
    shared_context_->event_mgr_->SetCurrentState(type);
    for (auto itr = states_.begin();
         itr != states_.end(); ++itr)
    {
        if (itr->first == type)
        {
            states_.back().second->Deactivate();
            StateType tmp_type = itr->first;
            BaseState* tmp_state = itr->second;
            states_.erase(itr);
            states_.emplace_back(tmp_type, tmp_state);
            tmp_state->Activate();
            shared_context_->wind_->GetRenderWindow()->setView(tmp_state->GetView());
            return;
        }
    }

    // State with l_type wasn't found.
    if (!states_.empty()) { states_.back().second->Deactivate(); }
    CreateState(type);
    states_.back().second->Activate();
    shared_context_->wind_->GetRenderWindow()->setView(states_.back().second->GetView());
}

void StateManager::Remove(const StateType& type)
{
    type_to_remove_.push_back(type);
}

// Private methods.

void StateManager::CreateState(const StateType& type)
{
    const auto newState = state_factory_.find(type);
    if (newState == state_factory_.end()) { return; }
    BaseState* state = newState->second();
    state->view_ = shared_context_->wind_->GetRenderWindow()->getDefaultView();
    states_.emplace_back(type, state);
    state->OnCreate();
}

void StateManager::RemoveState(const StateType& type)
{
    for (auto itr = states_.begin();
         itr != states_.end(); ++itr)
    {
        if (itr->first == type)
        {
            itr->second->OnDestroy();
            delete itr->second;
            states_.erase(itr);
            return;
        }
    }
}
