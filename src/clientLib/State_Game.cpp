#include "State_Game.h"
#include "StateManager.h"
#include "EntitySnapshot.h"

State_Game::State_Game(StateManager* _stateMgr) : BaseState(_stateMgr), player(-1), client(stateMgr->GetContext()->client),
    gameMap(nullptr) {};

State_Game::~State_Game() {};

void State_Game::OnCreate() {
    client->Setup(&State_Game::HandlePacket);
    if (client->Connect()) {
        stateMgr->GetContext()->sysMgr->GetSystem<S_Network>(System::Network)->SetClient(client);
        EventManager* evMgr = stateMgr->GetContext()->evMgr;
        evMgr->AddCallback(StateType::Game, "Key_Escape", &State_Game::MainMenu, this);
        evMgr->AddCallback(StateType::Game, "Player_MoveLeft", &State_Game::PlayerMove, this);
        evMgr->AddCallback(StateType::Game, "Player_MoveRight", &State_Game::PlayerMove, this);
        evMgr->AddCallback(StateType::Game, "Player_MoveUp", &State_Game::PlayerMove, this);
        evMgr->AddCallback(StateType::Game, "Player_MoveDown", &State_Game::PlayerMove, this);
        evMgr->AddCallback(StateType::Game, "Player_Attack", &State_Game::PlayerAttack, this);
        sf::Vector2u size = stateMgr->GetContext()->window->GetWindowSize();
        sf::View view;
        view.setSize(size.x, size.y);
        view.setCenter(size.x / 2, size.y / 2);
        view.zoom(0.6f);
        stateMgr->GetContext()->window->GetRenderWindow()->setView(view);
        gameMap = new Map(stateMgr->GetContext());
        gameMap->LoadMap("media/Maps/map1.map");

        EntityManager* entityMgr = stateMgr->GetContext()->entityMgr;
        stateMgr->GetContext()->sysMgr->GetSystem<S_Collision>(System::Collision)->SetMap(gameMap);
        stateMgr->GetContext()->sysMgr->GetSystem<S_Movement>(System::Movement)->SetMap(gameMap);
        stateMgr->GetContext()->soundMgr->PlayMusic("TownTheme", 50.f, true);
    } else {
        std::cout << "Failed to connect to the game server." << std::endl;
        stateMgr->Remove(StateType::Game);
        stateMgr->SwitchTo(StateType::MainMenu);
    }
};

void State_Game::OnDestroy() {
    client->Disconnect();
    client->UnregisterPacketHandler();
    S_Network* net = stateMgr->GetContext()->sysMgr->GetSystem<S_Network>(System::Network);
    net->ClearSnapshots();
    net->SetClient(nullptr);
    net->SetPlayerID((int)Network::NullID);
    stateMgr->GetContext()->entityMgr->Purge();
    EventManager* evMgr = stateMgr->GetContext()->evMgr;
    evMgr->RemoveCallback(StateType::Game, "Key_Escape");
    evMgr->RemoveCallback(StateType::Game, "Player_MoveLeft");
    evMgr->RemoveCallback(StateType::Game, "Player_MoveRight");
    evMgr->RemoveCallback(StateType::Game, "Player_MoveUp");
    evMgr->RemoveCallback(StateType::Game, "Player_MoveDown");
    evMgr->RemoveCallback(StateType::Game, "Player_Attack");
    if (gameMap) {
        delete gameMap;
        gameMap = nullptr;
    }
};

void State_Game::Update(const sf::Time& _time) {
    if (!client->IsConnected()) { 
        stateMgr->Remove(StateType::Game);
        stateMgr->SwitchTo(StateType::MainMenu);
        return;
    }
    SharedContext* context = stateMgr->GetContext();
    UpdateCamera();
    gameMap->Update(_time.asSeconds());
    sf::Lock lock(client->GetMutex());
    context->sysMgr->Update(_time.asMilliseconds());
};

void State_Game::UpdateCamera() {
    if (player == (int)Network::NullID) return;
    SharedContext* context = stateMgr->GetContext();
    C_Position* pos = context->entityMgr->GetComponent<C_Position>(player, Component::Position);
    view.setCenter(pos->GetPosition());
    context->window->GetRenderWindow()->setView(view);
    sf::FloatRect viewSpace = context->window->GetViewSpace();
    if (viewSpace.left <= 0) {
        view.setCenter(viewSpace.width / 2, view.getCenter().y);
        context->window->GetRenderWindow()->setView(view);
    } else if (viewSpace.left + viewSpace.width > (gameMap->GetMapSize().x * Sheet::Tile_Size)) {
        view.setCenter((gameMap->GetMapSize().x * Sheet::Tile_Size) - (viewSpace.width / 2), view.getCenter().y);
        context->window->GetRenderWindow()->setView(view);
    }
    if (viewSpace.top <= 0) {
        view.setCenter(view.getCenter().x, viewSpace.height / 2);
        context->window->GetRenderWindow()->setView(view);
    } else if (viewSpace.top + viewSpace.height > (gameMap->GetMapSize().y * Sheet::Tile_Size)) {
        view.setCenter(view.getCenter().x, (gameMap->GetMapSize().y * Sheet::Tile_Size) - (viewSpace.height / 2));
        context->window->GetRenderWindow()->setView(view);
    }
};

void State_Game::Draw() {
    if (!gameMap) return;
    sf::Lock lock(client->GetMutex());
    for (int i = 0; i < Sheet::Num_Layers; ++i) {
        gameMap->Draw(i);
        stateMgr->GetContext()->sysMgr->Draw(stateMgr->GetContext()->window, i);
    }
};

void State_Game::MainMenu(EventDetails* details) {
    stateMgr->SwitchTo(StateType::MainMenu);
};

void State_Game::Pause(EventDetails* details) {
    stateMgr->SwitchTo(StateType::Paused);
};

void State_Game::Activate() {};
void State_Game::Deactivate() {};

void State_Game::PlayerAttack(EventDetails* details) {
    Message msg((MessageType)EntityMessage::Move);
    if (details->name == "Player_MoveLeft") {
        msg._int = (int)Direction::Left;
    } else if (details->name == "Player_MoveRight") {
        msg._int = (int)Direction::Right;
    } else if (details->name == "Player_MoveUp") {
        msg._int = (int)Direction::Up;
    } else if (details->name == "Player_MoveDown") {
        msg._int = (int)Direction::Down;
    }
    msg.receiver = player;
    stateMgr->GetContext()->sysMgr->GetMessageHandler()->Dispatch(msg);
};

void State_Game::PlayerAttack(EventDetails* details) {
    Message msg((MessageType)EntityMessage::Attack);
    msg.receiver = player;
    stateMgr->GetContext()->sysMgr->GetMessageHandler()->Dispatch(msg);
};

void State_Game::HandlePacket(const PacketID& _pid, sf::Packet& _packet, Client* _client) {
    ClientEntityManager* entityMgr = (ClientEntityManager*)stateMgr->GetContext()->entityMgr;
    PacketType type = (PacketType)_pid;
    if (type == PacketType::Connect) {
        sf::Int32 eid;
        sf::Vector2f pos;
        if (!(_packet >> eid) || !(_packet >> pos.x) || !(_packet >> pos.y)) {
            std::cout << "[State_Game] Faulty CONNECT response" << std::endl;
            return;
        }
        std::cout << "Adding entity:" << eid << std::endl;
        client->GetMutex().lock();
        entityMgr->AddEntity("Player", eid);
        entityMgr->GetComponent<C_Position>(eid, Component::Position)->SetPosition(pos);
        client->GetMutex().unlock();
        player = eid;
        stateMgr->GetContext()->sysMgr->GetSystem<S_Network>(System::Network)->SetPlayerID(player);
        entityMgr->AddComponent(player, Component::SoundListener);
        return;
    }
    if (!client->IsConnected()) return;
    switch (type) {
    case PacketType::Snapshot:
    {
        sf::Int32 entityCount = 0;
        if (!(_packet >> entityCount)) std::cout << "[State_Game] Snapshot extraction failed." << std::endl;
        sf::Lock lock(client->GetMutex());
        sf::Int32 t = client->GetTime().asMilliseconds();
        for (unsigned int i = 0; i < entityCount; ++i) {
            sf::Int32 eid;
            EntitySnapshot snapshot;
            if (!(_packet >> eid) || !(_packet >> snapshot)) std::cout << "[State_Game] Snapshot extraction failed." << std::endl;
            stateMgr->GetContext()->sysMgr->GetSystem<S_Network>(System::Network)->AddSnapshot(eid, t, snapshot);
        }
        break;
    }
    case PacketType::Disconnect:
    {
        stateMgr->Remove(StateType::Game);
        stateMgr->SwitchTo(StateType::MainMenu);
        std::cout << "[State_Game] Disconnected by the server." << std::endl;
        break;
    }
    case PacketType::Hurt:
    {
        EntityID id;
        if (!(_packet >> id)) return;
        Message msg((MessageType)EntityMessage::Hurt);
        msg.receiver = id;
        stateMgr->GetContext()->sysMgr->GetMessageHandler()->Dispatch(msg);
        break;
    }
    }
};