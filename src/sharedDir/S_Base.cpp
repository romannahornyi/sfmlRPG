#include "S_Base.h"
#include "SystemManager.h"

S_Base::S_Base(const System& _id, SystemManager* _sysMgr) : id(_id), sysMgr(_sysMgr) {};

S_Base::~S_Base() { Purge(); };

bool S_Base::AddEntity(const EntityID& _entity) {
    if (HasEntity(_entity)) return false;
    entities.emplace_back(_entity);
    return true;
};

bool S_Base::RemoveEntity(const EntityID& _entity) { 
    auto itr = std::find_if(entities.begin(), entities.end(), [&_entity](const EntityID& _e) {
        return _e == _entity;
    });
    if (itr == entities.end()) return false;
    entities.erase(itr);
    return true;
};

bool S_Base::HasEntity(const EntityID& _entity) {
    return (std::find(entities.begin(), entities.end(), _entity) != entities.end());
};

System S_Base::GetID() {
    return id;
};

bool S_Base::FitsRequirements(const Bitmask& _bits) {
    return (std::find_if(requiredComponents.begin(), requiredComponents.end(), [&_bits](const Bitmask& _b){
        return _b.Matches(_bits, _b.GetMask());
    }) != requiredComponents.end());
};

void S_Base::Purge() { entities.clear(); };