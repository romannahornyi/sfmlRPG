#include "StateManager.h"
// #include "State_Intro.h"
// #include "State_MainMenu.h"
// #include "State_Game.h"
// #include "State_GameOver.h"
// #include "State_Paused.h"

StateManager::StateManager(SharedContext* _context) : shared(_context) {
    // RegisterState<State_Intro>(StateType::Intro);
    // RegisterState<State_MainMenu>(StateType::MainMenu);
    // RegisterState<State_Game>(StateType::Game);
    // RegisterState<State_Paused>(StateType::Paused);
    // RegisterState<State_GameOver>(StateType::GameOver);
};

StateManager::~StateManager() {
    for (auto& itr : states) {
        itr.second->OnDestroy();
        delete itr.second;
    }
};

void StateManager::Update(const sf::Time& _time) {
    if (states.empty()) return;
    if (states.back().second->IsTranscedent() && states.size() > 1) {
        auto itr = states.end();
        while (itr != states.begin()) {
            if (itr != states.end()) {
                if (!itr->second->IsTranscedent()) {
                    break;
                }
            }
            --itr;
        }
        for (; itr != states.end(); ++itr) {
            itr->second->Update(_time);
        }
    } else {
        states.back().second->Update(_time);
    }
};

void StateManager::Draw() {
    if (states.empty()) return; 
    if (states.back().second->IsTransparent() && states.size() > 1) {
        auto itr = states.end();
        while (itr != states.begin()) {
            if (itr != states.end()) {
                if (!itr->second->IsTransparent()) {
                    break;
                }
            }
        }
        for (; itr != states.end(); ++itr) {
            shared->window->GetRenderWindow()->setView(itr->second->GetView());
            itr->second->Draw();
        }
    } else {
        states.back().second->Draw();
    }
};

SharedContext* StateManager::GetContext() { return shared; };

bool StateManager::HasState(const StateType& _state) {
    for (auto itr = states.begin(); itr != states.end(); ++itr) {
        if (itr->first == _state) {
            auto itr2 = std::find(toRemove.begin(), toRemove.end(), itr->first);
            if (itr2 == toRemove.end()) return true;
            return false;
        }
    }
    return false;
};

void StateManager::ProcessRequests() {
    while (toRemove.begin() != toRemove.end()) {
        RemoveState(*toRemove.begin());
        toRemove.erase(toRemove.begin());
    }
};

void StateManager::SwitchTo(const StateType& _state) {
    shared->soundMgr->ChangeState(_state);
    shared->evMgr->SetCurrentState(_state);
    // shared->guiMgr->SetCurrentState(_state);
    for (auto itr = states.begin(); itr != states.end(); ++itr) {
        if (itr->first == _state) {
            states.back().second->Deactivate();
            StateType tmpType = itr->first;
            BaseState* tmpState = itr->second;
            states.erase(itr);
            states.emplace_back(std::make_pair(tmpType, tmpState));
            tmpState->Activate();
            shared->window->GetRenderWindow()->setView(tmpState->GetView());
            return;
        }
    }
    if (!states.empty()) states.back().second->Deactivate();
    CreateState(_state);
    states.back().second->Activate();
    shared->window->GetRenderWindow()->setView(states.back().second->GetView());
};

void StateManager::Remove(const StateType& _state) {
    toRemove.emplace_back(_state);
};

void StateManager::CreateState(const StateType& _state) {
    auto newState = stateFactory.find(_state);
    if (newState == stateFactory.end()) return;
    BaseState* state = newState->second();
    state->view = shared->window->GetRenderWindow()->getDefaultView();
    states.emplace_back(_state, state);
    state->OnCreate();
};

void StateManager::RemoveState(const StateType& _state) {
    for (auto itr = states.begin(); itr != states.end(); ++itr) {
        if (itr->first == _state) {
            itr->second->OnDestroy();
            states.erase(itr);
            delete itr->second;
            shared->soundMgr->RemoveState(_state);
            return;
        }
    }
};