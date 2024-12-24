#pragma once
#include "S_Base.h"
#include "C_Movable.h"
#include "C_Position.h"
#include "Directions.h"
#include <cmath>

enum class Axis { x, y };

class Map;

class S_Movement : public S_Base {
public:
    S_Movement(SystemManager* _sysMgr);
    ~S_Movement();
    
    void Update(float _dt);
    void HandleEvent(const EntityID& _entity, const EntityEvent& _event);
    void Notify(const Message& _msg);

    void SetMap(Map* _map);
    void StopEntity(const EntityID& _entity, const Axis& _axis);
    void SetDirection(const EntityID& _entity, const Direction& _dir);
private:
    const sf::Vector2f& GetTileFriction(unsigned int _layer, unsigned int _x, unsigned int _y);
    void MovementStep(float _dt, C_Movable* _mov, C_Position* _pos);
    Map* gameMap;
}; 