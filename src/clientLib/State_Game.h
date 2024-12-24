#pragma once
#include "BaseState.h"
#include "Map.h"
#include "Client.h"
#include <unordered_map>

class State_Game : public BaseState {
public:
    State_Game(StateManager* _stateMgr);
    ~State_Game();

    void Activate();
    void Deactivate();

    void OnCreate();
    void OnDestroy();

    void Update(const sf::Time& _time);
    void Draw();

    void HandlePacket(const PacketID& _pid, sf::Packet& _packet, Client* _client);
    
    void MainMenu(EventDetails* _details);
    void Pause(EventDetails* _details);
    void PlayerMove(EventDetails* _details);
    void PlayerAttack(EventDetails* _details);
private:
    void UpdateCamera();
    Map* gameMap;
    int player;
    Client* client;
};