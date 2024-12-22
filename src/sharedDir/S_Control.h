#pragma once
#include "S_Base.h"
#include "C_Movable.h"
#include "Directions.h"

class S_Control : public S_Base {
public:
    S_Control(SystemManager* _sysMgr);
    ~S_Control();

    void Update(float _dt);
    void HandleEvent(const EntityID& _entity, const EntityEvent& _event);
    void Notify(const Message& _msg);

    void MoveEntity(const EntityID& _entity, const Direction& _dir);
private:

};