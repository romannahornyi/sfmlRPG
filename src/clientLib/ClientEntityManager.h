#pragma once
#include "C_Position.h"
#include "C_State.h"
#include "C_Movable.h"
#include "C_Collidable.h"
#include "C_Controller.h"
#include "C_SpriteSheet.h"
#include "C_SoundEmitter.h"
#include "C_SoundListener.h"
#include "C_Name.h"
#include "C_Health.h"
#include "C_Client.h"
#include "C_UI_Element.h"
#include "EntityManager.h"
#include "TextureManager.h"
#include <vector>

class SystemManager; 

class ClientEntityManager : public EntityManager {
public:
    ClientEntityManager(SystemManager* _sysMgr, TextureManager* _textMgr);
    ~ClientEntityManager();
    int AddEntity(const std::string& _entityFile, int _id = -1);
protected:
    TextureManager* textMgr;
};