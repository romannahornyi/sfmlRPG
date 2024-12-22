#pragma once
#include "S_Base.h"
#include "C_State.h"
#include "Directions.h"

class S_State : public S_Base {
public:
    S_State(SystemManager* _sysMgr);
    ~S_State();

    void Update(float _dt);
    void HandleEvent(const EntityID& _entity, const EntityEvent& _event);
    void Notify(const Message& _msg);

    EntityState GetState(const EntityID& _entity);
    void ChangeState(const EntityID& _entity, const EntityState& _state, bool _force);
private:

};