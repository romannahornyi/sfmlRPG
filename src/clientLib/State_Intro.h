#pragma once
#include "BaseState.h"
#include "EventManager.h"

class State_Intro : public BaseState {
public:
    State_Intro(StateManager* _stateMgr);
    ~State_Intro();

    void OnCreate();
    void OnDestroy();

    void Activate();
    void Deactivate();

    void Update(const sf::Time& _time);
    void Draw();

    void Continue(EventDetails* _details);
private:
    sf::Sprite introSprite;
    sf::Text text;
};