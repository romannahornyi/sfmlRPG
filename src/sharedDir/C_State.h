#pragma once
#include "C_Base.h"

enum class EntityState {Idle, Walking, Attacking, Hurt, Dying};

class C_State : public C_Base {
public:
    C_State() : C_Base(Component::State) {};
    void ReadIn(std::stringstream& _stream) {
        unsigned int _state;
        _stream >> _state;
        state = (EntityState)_state;
    };

    void SetState(const EntityState& _state) { state = _state; };
    EntityState GetState() { return state; };
private:
    EntityState state;
};