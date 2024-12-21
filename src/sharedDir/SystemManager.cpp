#include "SystemManager.h"
#include "EntityManager.h"

SystemManager::SystemManager() { entityMgr = nullptr; };

SystemManager::~SystemManager() { PurgeSystems(); };

void SystemManager::SetEntityManager(EntityManager* _entityMgr) {
    if (!entityMgr) entityMgr = _entityMgr;
};

EntityManager* SystemManager::GetEntityManager() { return entityMgr; };
MessageHandler* SystemManager::GetMessageHandler() { return &messages; };

void SystemManager::AddEvent(const EntityID& _entity, const EventID& _event) {
    events[_entity].AddEvent(_event);
};

void SystemManager::Update(float _dt) {
    for (auto& s : systems) {
        s.second->Update(_dt);
    }
    HandleEvents();
};

void SystemManager::HandleEvents() {
    for (auto& itr : events) {
        EventID eid = 0;
        while (itr.second.ProcessEvents(eid)) {
            for (auto& s : systems) {
                if (s.second->HasEntity(itr.first)) {
                    s.second->HandleEvent(itr.first, (EntityEvent)eid);
                }
            }
        }
    }
};

void SystemManager::EntityModified(const EntityID& _entity, const Bitmask& _bits) {
    for (auto& itr : systems) {
        S_Base* s = itr.second;
        if (s->FitsRequirements(_bits)) {
            if (!s->HasEntity(_entity)) {
                s->AddEntity(_entity);
            }
        } else {
            if (s->HasEntity(_entity)) {
                s->RemoveEntity(_entity);
            }
        }
    }
};

void SystemManager::RemoveEntity(const EntityID& _entity) {
    for (auto& itr : systems) {
        itr.second->RemoveEntity(_entity);
    }
};

void SystemManager::PurgeEntities() {
    for (auto& itr : systems) {
        itr.second->Purge();
    }
};

void SystemManager::PurgeSystems() {
    for (auto& itr : systems) {
        delete itr.second;
    }
    systems.clear();
};