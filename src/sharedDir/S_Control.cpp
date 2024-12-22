#include "S_Control.h"
#include "SystemManager.h"

S_Control::S_Control(SystemManager* _sysMgr) : S_Base(System::Control, _sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::Position);
    req.TurnOnBit((unsigned int)Component::Movable);
    req.TurnOnBit((unsigned int)Component::Controller);
    requiredComponents.push_back(req);
    req.Clear();
};

S_Control::~S_Control() {};

void S_Control::Update(float _dt) {};

void S_Control::HandleEvent(const EntityID& _entity, const EntityEvent& _event) {
    switch (_event) {
        case EntityEvent::Moving_Left:
            MoveEntity(_entity, Direction::Left);
            break;
        case EntityEvent::Moving_Right:
            MoveEntity(_entity, Direction::Right);
            break;
        case EntityEvent::Moving_Up:
            MoveEntity(_entity, Direction::Up);
            break;
        case EntityEvent::Moving_Down:
            MoveEntity(_entity, Direction::Down);
            break;
    };
};

void S_Control::Notify(const Message& _msg) {};

void S_Control::MoveEntity(const EntityID& _entity, const Direction& _dir) {
    C_Movable* mov = sysMgr->GetEntityManager()->GetComponent<C_Movable>(_entity, Component::Movable);
    mov->Move(_dir);
};