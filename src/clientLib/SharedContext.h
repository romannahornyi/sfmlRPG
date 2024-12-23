#pragma once
#include "Window.h"
#include "StateManager.h"
#include "EventManager.h"
#include "Map.h"
#include "Client.h"
#include "TextureManager.h"
#include "FontManager.h"
#include "SoundManager.h"
#include "AudioManager.h"
// #include "GUI_Manager.h"
// #include "ClientSystemManager.h"
#include "ClientEntityManager.h"

struct SharedContext {
    SharedContext() : window(nullptr), evMgr(nullptr), textMgr(nullptr), gameMap(nullptr),
        fontMgr(nullptr), soundMgr(nullptr), audioMgr(nullptr), client(nullptr), entityMgr(nullptr)
        //, sysMgr(nullptr), guiMgr(nullptr), stateMgr(nullptr) 
        {};
    Window* window;
    EventManager* evMgr;
    TextureManager* textMgr;
    FontManager* fontMgr;
    SoundManager* soundMgr;
    AudioManager* audioMgr;
    Client* client;
    Map* gameMap;
    // ClientSystemManager* sysMgr;
    ClientEntityManager* entityMgr;
    // GUI_Manager* guiMgr;
    // StateManager* stateMgr;
};