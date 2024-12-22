#include "EntityManager.h"
#include "SystemManager.h"

EntityManager::EntityManager(SystemManager* _sysMgr) : sysMgr(_sysMgr), idCounter(0) {};
EntityManager::~EntityManager() { Purge(); };

void EntityManager::SetSystemManager(SystemManager* _sysMgr) {
    sysMgr = _sysMgr;
};

int EntityManager::AddEntity(Bitmask& _bitmask, int _id) {
    unsigned int entity = (_id == -1 ? idCounter : _id);
    if (!entities.emplace(entity, EntityData()).second) return -1;
    if (_id == -1) ++idCounter;
    for (int i = 0; i < N_COMPONENTS; ++i) {
        if (_bitmask.GetBit(i)) {
            AddComponent(entity, (Component)i);
        }
    }
    sysMgr->EntityModified(entity, _bitmask);
    sysMgr->AddEvent(entity, (EventID)EntityEvent::Spawned);
    return entity;
};

int EntityManager::AddEntity(const std::string& _fileName, int _id) {
    int entityID = -1;
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + "media/Entities/" + _fileName + ".entity");
    if (!file.is_open()) {
        std::cout << "[EntityManager] Failed to open entity from file: " << _fileName << std::endl;
        return -1;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "Name") {

        } else if (type == "Attributes") {
            if (entityID != -1) continue;
            Bitmask mask;
            Bitset set;
            keystream >> set;
            mask.SetMask(set);
            entityID = AddEntity(mask);
            if (entityID == -1) return -1;
        } else if (type == "Component") {
            if (entityID == -1) continue;
            unsigned int cid = 0;
            keystream >> cid;
            C_Base* c = GetComponent<C_Base>(entityID, (Component)cid);
            keystream >> *c;
        }
    }
    file.close();
    entities.at(entityID).type = _fileName;
    return entityID;
};

bool EntityManager::RemoveEntity(const EntityID& _entity) {
    auto itr = entities.find(_entity);
    if (itr == entities.end()) return false;
    Message msg((MessageType)EntityMessage::Removed_Entity);
    msg.receiver = _entity;
    msg._int = _entity;
    sysMgr->GetMessageHandler()->Dispatch(msg);
    while (itr->second.components.begin() != itr->second.components.end()) {
        delete itr->second.components.back();
        itr->second.components.pop_back();
    }
    entities.erase(itr);
    sysMgr->RemoveEntity(_entity);
    return true;
};

bool EntityManager::AddComponent(const EntityID& _entity, const Component& _comp) {
    auto itr = entities.find(_entity);
    if (itr == entities.end()) return false;
    if (itr->second.mask.GetBit((unsigned int)_comp)) return false;
    auto itr2 = compFactory.find(_comp);
    if (itr2 == compFactory.end()) return false;
    C_Base* c = itr2->second();
    itr->second.components.emplace_back(c);
    itr->second.mask.TurnOnBit((unsigned int)_comp);
    sysMgr->EntityModified(_entity, itr->second.mask);
    return true;
};

bool EntityManager::HasEntity(const EntityID& _entity) {
    return (entities.find(_entity) != entities.end());
};

bool EntityManager::RemoveComponent(const EntityID& _entity, const Component& _comp) {
    auto itr = entities.find(_entity);
    if (itr == entities.end()) return false;
    if (!itr->second.mask.GetBit((unsigned int)_comp)) return false;
    auto& container = itr->second.components;
    auto c = std::find_if(container.begin(), container.end(), [&_comp](C_Base* _c){
        return _c->GetType() == _comp;
    });
    if (c == container.end()) return false;
    delete (*c);
    container.erase(c);
    itr->second.mask.ClearBit((unsigned int)_comp);
    sysMgr->EntityModified(_entity, itr->second.mask);
    return true;
};

bool EntityManager::HasComponent(const EntityID& _entity, const Component& _comp) {
    auto itr = entities.find(_entity);
    if (itr == entities.end()) return false;
    return itr->second.mask.GetBit((unsigned int)_comp);
};

void EntityManager::Purge() {
    if (sysMgr != nullptr) sysMgr->PurgeEntities();
    for (auto& itr : entities) {
        for (auto& c : itr.second.components) {
            delete c;
        }
        itr.second.components.clear();
        itr.second.mask.Clear();
    }
    entities.clear();
    idCounter = 0;
};

unsigned int EntityManager::GetEntityCount() { return (int)entities.size(); };