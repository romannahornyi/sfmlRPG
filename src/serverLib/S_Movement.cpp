#include "S_Movement.h"
#include "SystemManager.h"
#include "Map.h"

S_Movement::S_Movement(SystemManager* _sysMgr) : S_Base(System::Movement, _sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::Movable);
    req.TurnOnBit((unsigned int)Component::Position);
    requiredComponents.push_back(req);
    req.Clear();

    sysMgr->GetMessageHandler()->Subscribe(EntityMessage::Is_Moving, this);
    gameMap = nullptr;
};

S_Movement::~S_Movement() {};

void S_Movement::Update(float _dt) {
    if (!gameMap) return;
    EntityManager* entityMgr = sysMgr->GetEntityManager();
    for (auto& entity : entities) {
        C_Position* pos = entityMgr->GetComponent<C_Position>(entity, Component::Position);
        C_Movable* mov = entityMgr->GetComponent<C_Movable>(entity, Component::Movable);
        MovementStep(_dt, mov, pos);
        pos->MoveBy(_dt * mov->GetVelocity());
    }
};

void S_Movement::HandleEvent(const EntityID& _entity, const EntityEvent& _event) {
    switch (_event) {
    case EntityEvent::Colliding_X: StopEntity(_entity, Axis::x); break;
    case EntityEvent::Colliding_Y: StopEntity(_entity, Axis::y); break;
    case EntityEvent::Moving_Left: SetDirection(_entity, Direction::Left); break;
    case EntityEvent::Moving_Right: SetDirection(_entity, Direction::Right); break;
    case EntityEvent::Moving_Up:
        C_Movable* mov = sysMgr->GetEntityManager()->GetComponent<C_Movable>(_entity, Component::Movable);
        if (mov->GetVelocity().x == 0) SetDirection(_entity, Direction::Up);
        break;
    case EntityEvent::Moving_Down:
        C_Movable* mov = sysMgr->GetEntityManager()->GetComponent<C_Movable>(_entity, Component::Movable);
        if (mov->GetVelocity().x == 0) SetDirection(_entity, Direction::Down);
        break; 
    }
};

void S_Movement::Notify(const Message& _msg) {
    EntityMessage m = (EntityMessage)_msg.type;
    switch (m) {
    case EntityMessage::Is_Moving:
        if (!HasEntity(_msg.receiver)) return;
        C_Movable* mov = sysMgr->GetEntityManager()->GetComponent<C_Movable>(_msg.receiver, Component::Movable);
        if (mov->GetVelocity() != sf::Vector2f(0.f, 0.f)) return;
        sysMgr->AddEvent(_msg.receiver, (EventID)EntityEvent::Began_Idle);
        break;
    }
};

const sf::Vector2f& S_Movement::GetTileFriction(unsigned int _layer, unsigned int _x, unsigned int _y) {
    Tile* t = nullptr;
    while (!t && _layer >= 0) {
        t = gameMap->GetTile(_x, _y, _layer);
        --_layer;
    }
    return (t ? t->properties->friction : gameMap->GetDefaultTile()->friction);
};

void S_Movement::MovementStep(float _dt, C_Movable* mov, C_Position* pos) {
    sf::Vector2f coef = GetTileFriction(pos->GetElevation(),
        std::floor(pos->GetPosition().x / Sheet::Tile_Size),
        std::floor(pos->GetPosition().y / Sheet::Tile_Size));

    sf::Vector2f friction(mov->GetSpeed().x * coef.x, mov->GetSpeed().y * coef.y);

    mov->AddVelocity(mov->GetAcceleration() * _dt);
    mov->SetAcceleration(sf::Vector2f(0.f, 0.f));
    mov->ApplyFriction(friction * _dt);

    float magnitude = std::sqrt(
        (mov->GetVelocity().x * mov->GetVelocity().x) + 
        (mov->GetVelocity().y * mov->GetVelocity().y)
    );

    if (magnitude <= mov->GetVelocityMax()) return;
    float maxV = mov->GetVelocityMax();
    mov->SetVelocity(sf::Vector2f(
        (mov->GetVelocity().x / magnitude) * maxV,
        (mov->GetVelocity().y / magnitude) * maxV
    ));
};

void S_Movement::SetMap(Map* _map) { gameMap = _map; };

void S_Movement::StopEntity(const EntityID& _entity, const Axis& _axis) {
    C_Movable* mov = sysMgr->GetEntityManager()->GetComponent<C_Movable>(_entity, Component::Movable);
    if (_axis == Axis::x) {
        mov->SetVelocity(sf::Vector2f(0.f, mov->GetVelocity().y));
    } else if (_axis == Axis::y) {
        mov->SetVelocity(sf::Vector2f(mov->GetVelocity().x, 0.f));
    }
};

void S_Movement::SetDirection(const EntityID& _entity, const Direction& _dir) {
    C_Movable* mov = sysMgr->GetEntityManager()->GetComponent<C_Movable>(_entity, Component::Movable);
    mov->SetDirection(_dir);

    Message msg((MessageType)EntityMessage::Direction_Changed);
    msg.receiver = _entity;
    msg._int = (int)_dir;
    sysMgr->GetMessageHandler()->Dispatch(msg);
};