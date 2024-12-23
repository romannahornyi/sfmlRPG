#include "ClientEntityManager.h"

ClientEntityManager::ClientEntityManager(SystemManager* _sysMgr, TextureManager* _textMgr) : EntityManager(_sysMgr),
    textMgr(_textMgr) {
    AddComponentType<C_Position>(Component::Position);
    AddComponentType<C_State>(Component::State);
    AddComponentType<C_Movable>(Component::Movable);
    AddComponentType<C_Collidable>(Component::Collidable);
    AddComponentType<C_SpriteSheet>(Component::SpriteSheet);
    AddComponentType<C_Client>(Component::Client);
    AddComponentType<C_Controller>(Component::Controller);
    AddComponentType<C_Name>(Component::Name);
    AddComponentType<C_Health>(Component::Health);
    AddComponentType<C_SoundEmitter>(Component::SoundEmitter);
    AddComponentType<C_SoundListener>(Component::SoundListener);
    AddComponentType<C_UI_Element>(Component::UI_Element);
};

ClientEntityManager::~ClientEntityManager() {};

int ClientEntityManager::AddEntity(const std::string& _fileName, int _id) {
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
            entityID = EntityManager::AddEntity(mask);
            if (entityID == -1) return -1;
        } else if (type == "Component") {
            if (entityID == -1) continue;
            unsigned int cid = 0;
            keystream >> cid;
            C_Base* c = GetComponent<C_Base>(entityID, (Component)cid);
            keystream >> *c;
            if (c->GetType() == Component::SpriteSheet) {
                C_SpriteSheet* sheet = (C_SpriteSheet*)c;
                sheet->Create(textMgr);
            }
        }
    }
    file.close();
    return entityID;
};