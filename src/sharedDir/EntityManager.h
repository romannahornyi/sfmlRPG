#pragma once
#include "Bitmask.h"
#include "C_Base.h"
#include <vector>
#include <unordered_map>
#include <fstream>
#include <functional>
#include "Utilities.h"

using EntityID = int;
using ComponentContainer = std::vector<C_Base*>;

struct EntityData {
    Bitmask mask;
    std::string type;
    ComponentContainer components;
};

using EntityContainer = std::unordered_map<EntityID, EntityData>;
using ComponentFactory = std::unordered_map<Component, std::function<C_Base*(void)>>;

class SystemManager;

class EntityManager {
public:
    EntityManager(SystemManager* _sysMgr);
    virtual ~EntityManager();

    void SetSystemManager(SystemManager* _sysMgr);

    int AddEntity(Bitmask& _mask, int _id = -1);
    virtual int AddEntity(const std::string& _file, int _id = -1);

    bool RemoveEntity(const EntityID& _id);
    bool HasEntity(const EntityID& _id);

    bool AddComponent(const EntityID& _entity, const Component& _comp);
    template <class T>
    T* GetComponent(const EntityID& _entity, const Component& _comp) {
        auto entity = entities.find(_entity);
        if (entity == entities.end()) return nullptr;
        if (!entity->second.mask.GetBit((unsigned int)_comp)) return nullptr;
        auto& container = entity->second.components;
        auto itr = std::find_if(container.begin(), container.end(), [&_comp](C_Base* _c){
            return _c->GetType() == _comp;
        });
        return (itr != container.end() ? dynamic_cast<T*>(*itr) : nullptr);
    };

    bool RemoveComponent(const EntityID& _entity, const Component& _comp);
    bool HasComponent(const EntityID& _entity, const Component& _comp);

    void Purge();
    unsigned int GetEntityCount();
protected:
    template <class T>
    void AddComponentType(const Component& _comp) {
        compFactory[_comp] = []()->C_Base* {
            return new T();
        };
    };

    EntityContainer entities;
    ComponentFactory compFactory;
    unsigned int idCounter;
    SystemManager* sysMgr;
};