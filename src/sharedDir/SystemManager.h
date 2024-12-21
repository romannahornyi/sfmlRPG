#pragma once
#include <SFML/Graphics.hpp>
#include "EntityEvents.h"
#include "MessageHandler.h"
#include <unordered_map>
#include "S_Base.h"

using SystemContainer = std::unordered_map<System, S_Base*>;
using EntityEventContainer = std::unordered_map<EntityID, EventQueue>;

class EntityManager;

class SystemManager {
public:
    SystemManager();
    virtual ~SystemManager();

    void SetEntityManager(EntityManager* _entityMgr);
    EntityManager* GetEntityManager();
    MessageHandler* GetMessageHandler();

    template <class T>
    T* GetSystem(const System& _sys) {
        auto itr = systems.find(_sys);
        return (itr != systems.end() ? dynamic_cast<T*>(itr->second) : nullptr);
    };
    template <class T>
    bool AddSystem(const System& _sys) {
       if (systems.find(_sys) != systems.end()) return false;
        systems[_sys] = new T(this);
        return true;
    };

    void AddEvent(const EntityID& _entity, const EventID& _event);
    void Update(float _dt);
    void HandleEvents();

    void EntityModified(const EntityID& _entity, const Bitmask& _bitmask);
    void RemoveEntity(const EntityID& _entity);

    void PurgeEntities();
    void PurgeSystems();
private:
    EntityManager* entityMgr;
    SystemContainer systems;
    EntityEventContainer events;
    MessageHandler messages;
};