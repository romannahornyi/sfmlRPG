#include "ServerEntityManager.h"
#include "SystemManager.h"

ServerEntityManager::ServerEntityManager(SystemManager* _sysMgr) : EntityManager(_sysMgr) {
    AddComponentType<C_Attacker>(Component::Attacker);
    AddComponentType<C_Position>(Component::Position);
    AddComponentType<C_Health>(Component::Health);
    AddComponentType<C_Name>(Component::Name);
    AddComponentType<C_Client>(Component::Client);
    AddComponentType<C_State>(Component::State);
    AddComponentType<C_Collidable>(Component::Collidable);
    AddComponentType<C_Controller>(Component::Controller);
    AddComponentType<C_Movable>(Component::Movable);
};

ServerEntityManager::~ServerEntityManager() {};

void ServerEntityManager::DumpEntityInfo(sf::Packet& _packet) {
    for (auto& entity : entities) {
        _packet << sf::Int32(entity.first);
        EntitySnapshot snapshot;
        snapshot.type = entity.second.type;
        auto& mask = entity.second.mask;
        if (mask.GetBit((unsigned int)Component::Position)) {
            C_Position* pos = GetComponent<C_Position>(entity.first, Component::Position);
            snapshot.position = pos->GetPosition();
            snapshot.elevation = pos->GetElevation();
        }
        if (mask.GetBit((unsigned int)Component::Movable)) {
            C_Movable* mov = GetComponent<C_Movable>(entity.first, Component::Movable);
            snapshot.velocity = mov->GetVelocity();
            snapshot.acceleration = mov->GetAcceleration();
        }
        if (mask.GetBit((unsigned int)Component::State)) {
            C_State* state = GetComponent<C_State>(entity.first, Component::State);
            snapshot.state = sf::Uint8(state->GetState());
        }
        if (mask.GetBit((unsigned int)Component::Health)) {
            C_Health* health = GetComponent<C_Health>(entity.first, Component::Health);
            snapshot.health = health->GetHealth();
        }
        if (mask.GetBit((unsigned int)Component::Name)) {
            C_Name* name = GetComponent<C_Name>(entity.first, Component::Name);
            snapshot.name = name->GetName();
        }
        _packet << snapshot;
    };
};