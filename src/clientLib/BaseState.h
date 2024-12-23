#pragma once
#include <SFML/Graphics.hpp>

class StateManager;

class BaseState {
    friend class StateManager;
public:
    BaseState(StateManager* _mgr) : stateMgr(_mgr), transparent(false), transcedent(false) {};
    virtual ~BaseState() {};

    virtual void OnCreate() = 0;
    virtual void OnDestroy() = 0;
    
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;

    virtual void Update(const sf::Time& _time) = 0;
    virtual void Draw() = 0;

    void SetTransparent(bool _value) { transparent = _value; };
    bool IsTransparent() const { return transparent; };

    void SetTranscedent(bool _value) { transcedent = _value; };
    bool IsTranscedent() const { return transcedent; };

    sf::View& GetView() { return view; };
    StateManager* GetStateManager() { return stateMgr; };
protected:
    StateManager* stateMgr;
    bool transparent;
    bool transcedent;
    sf::View view;
};