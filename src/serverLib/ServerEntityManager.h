#pragma once
#include "EntityManager.h"
#include "C_Attacker.h"
#include "C_Position.h"
#include "C_Movable.h"
#include "C_Collidable.h"
#include "C_Client.h"
#include "C_Name.h"
#include "C_Health.h"
#include "C_State.h"
#include "C_Controller.h"
#include "EntitySnapshot.h"

class SystemManager;

class ServerEntityManager : public EntityManager {
public:
    ServerEntityManager(SystemManager* _sysMgr);
    ~ServerEntityManager();
    void DumpEntityInfo(sf::Packet& _packet);
private:

};