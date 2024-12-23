#pragma once
#include "BaseState.h"
#include "SharedContext.h"
#include <vector>
#include <unordered_map>

enum class StateType { Intro = 1, MainMenu, Game, Paused, GameOver, Credits };

using TypeContainer = std::vector<StateType>;
using StateContainer = std::vector<std::pair<StateType, BaseState*>>;
using StateFactory = std::unordered_map<StateType, std::function<BaseState*(void)>>;

class StateManager {
public:
    StateManager(SharedContext* _context);
    ~StateManager();

    void Update(const sf::Time& _time);
    void Draw();

    void ProcessRequests();

    SharedContext* GetContext();
    bool HasState(const StateType& _state);

    void SwitchTo(const StateType& _state);
    void Remove(const StateType& _state);
private:
    void CreateState(const StateType& _state);
    void RemoveState(const StateType& _state);

    template<class T>
    void RegisterState(const StateType& _state) {
        stateFactory[_state] = [this]()->BaseState*{
            return new T(this);
        };
    };
    SharedContext* shared;
    TypeContainer toRemove;
    StateContainer states;
    StateFactory stateFactory;
};