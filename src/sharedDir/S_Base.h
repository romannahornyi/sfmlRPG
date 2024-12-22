#pragma once
#include <vector>
#include "Bitmask.h"
#include "Observer.h"
#include "EventQueue.h"
#include "ECS_Types.h"
#include "EntityEvents.h"
#include "EntityManager.h"

using EntityList = std::vector<EntityID>;
using Requirements = std::vector<Bitmask>;

class SystemManager;

class S_Base : public Observer {
public:
    S_Base(const System& _sys, SystemManager* _sysMgr);
    virtual ~S_Base();

    bool AddEntity(const EntityID& _entity);
    bool RemoveEntity(const EntityID& _entity);
    bool HasEntity(const EntityID& _entity);

    System GetID();

    bool FitsRequirements(const Bitmask& _bits);
    void Purge();

    virtual void HandleEvent(const EntityID& _entity, const EntityEvent& _event) = 0;
    virtual void Update(float _dt) = 0;
protected:
    SystemManager* sysMgr;
    EntityList entities;
    Requirements requiredComponents;

    System id;
};