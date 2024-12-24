#include "S_Network.h"
#include "SystemManager.h"
#include "S_Renderer.h"

void InterpolateSnapshot(const EntitySnapshot& s1, const sf::Int32& t1, const EntitySnapshot& s2,
    const sf::Int32& t2, EntitySnapshot& target, const sf::Int32& tx) {
    target.name = s2.name;
    target.health = s2.health;
    target.type = s2.type;
    target.direction = s2.direction;
    target.state = s2.state;

    target.acceleration.x = InterpolateSnapshot<float>(t1, t2, s1.acceleration.x, s2.acceleration.x, tx);
    target.acceleration.y = InterpolateSnapshot<float>(t1, t2, s1.acceleration.y, s2.acceleration.y, tx);

    target.velocity.x = InterpolateSnapshot<float>(t1, t2, s1.velocity.x, s2.velocity.x, tx);
    target.velocity.y = InterpolateSnapshot<float>(t1, t2, s1.velocity.y, s2.velocity.y, tx);

    target.position.x = InterpolateSnapshot<float>(t1, t2, s1.position.x, s2.position.x, tx);
    target.position.y = InterpolateSnapshot<float>(t1, t2, s1.position.y, s2.position.y, tx);
};

bool CompareSnapshot(const EntitySnapshot& _f, const EntitySnapshot& _s, bool position, 
    bool physics, bool state) {
    if (position && (_f.position != _s.position)) return false;
    if (physics && (_f.acceleration != _s.acceleration ||
        _f.velocity != _s.velocity)) return false;
    if (state && (_f.state != _s.state)) return false;
    return true; 
};

S_Network::S_Network(SystemManager* _sysMgr) : S_Base(System::Network, _sysMgr), client(nullptr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::Client);
    requiredComponents.push_back(req);
    req.Clear();

    sysMgr->GetMessageHandler()->Subscribe(EntityMessage::Attack, this);
    sysMgr->GetMessageHandler()->Subscribe(EntityMessage::Move, this);
    playerUpdateTimer = sf::milliseconds(0);
};

S_Network::~S_Network() {};

void S_Network::Update(float _dt) {
    if (!client) return;
    playerUpdateTimer += sf::milliseconds(_dt);
    sf::Lock lock(client->GetMutex());
    if (playerUpdateTimer.asMilliseconds() >= PLAYER_UPDATE_INTERVAL) {
        SendPlayerOutgoing();
        playerUpdateTimer = sf::milliseconds(0);
    }
    PerformInterpolation();
};

void S_Network::HandleEvent(const EntityID& _entity, const EntityEvent& _event) {};

void S_Network::Notify(const Message& _msg) {
    if (!HasEntity(_msg.receiver) || _msg.receiver != player) return;
    if ((EntityMessage)_msg.type == EntityMessage::Attack && outgoing.find(EntityMessage::Attack) != outgoing.end()) return;
    outgoing[(EntityMessage)_msg.type].emplace_back(_msg);
};

void S_Network::SetClient(Client* _client) { client = _client; };

void S_Network::SetPlayerID(const EntityID& _entity) { player = _entity; };

void S_Network::AddEntitySnapshot(const EntityID& _entity, const sf::Int32& _timestamp, EntitySnapshot& _snapshot) {
    sf::Lock lock(client->GetMutex());
    auto itr = entitySnapshots.emplace(_timestamp, SnapshotDetails());
    itr.first->second.snapshots.emplace(_entity, _snapshot);
};

void S_Network::ApplyEntitySnapshot(const EntityID& _entity, const EntitySnapshot& _snapshot, bool _applyPhysics) {
    ClientEntityManager* entityMgr = (ClientEntityManager*)sysMgr->GetEntityManager();
    C_Position* pos = nullptr;
    C_Movable* mov = nullptr;
    C_Name* name = nullptr;
    C_Health* health = nullptr;
    S_Movement* movs = nullptr;
    S_State* states = nullptr;
    sf::Lock lock(client->GetMutex());
    if (pos = entityMgr->GetComponent<C_Position>(_entity, Component::Position)) {
        pos->SetPosition(_snapshot.position);
        pos->SetElevation(_snapshot.elevation);
    }
    if (_applyPhysics) {
        if (mov = entityMgr->GetComponent<C_Movable>(_entity, Component::Movable)) {
            mov->SetVelocity(_snapshot.velocity);
            mov->SetAcceleration(_snapshot.acceleration);
        }
    }
    if (name = entityMgr->GetComponent<C_Name>(_entity, Component::Name)) {
        name->SetName(_snapshot.name);
    }
    if (health = entityMgr->GetComponent<C_Health>(_entity, Component::Health)) {
        health->SetHealth(_snapshot.health);
    }
    if (states = sysMgr->GetSystem<S_State>(System::State)) {
        states->ChangeState(_entity, (EntityState)_snapshot.state, true);
    }
    if (movs = sysMgr->GetSystem<S_Movement>(System::Movement)) {
        movs->SetDirection(_entity, (Direction)_snapshot.direction);
    }
};

void S_Network::SendPlayerOutgoing() {
    sf::Int32 px = 0, py = 0;
    sf::Int8 pa = 0;
    for (auto& itr : outgoing) {
        if (itr.first == EntityMessage::Move) {
            sf::Int32 x = 0, y = 0;
            for (auto& msg : itr.second) {
                if (msg._int == (int)Direction::Left) --x;
                else if (msg._int == (int)Direction::Right) ++x;
                else if (msg._int == (int)Direction::Up) --y;
                else if (msg._int == (int)Direction::Down) ++y;
            }
            if (!x && !y) continue;
            px = x; py = y;
        } else if (itr.first == EntityMessage::Attack) {
            pa = 1;
        }
    }
    sf::Packet packet;
    StampPacket(PacketType::Player_Update, packet);
    packet << sf::Int8(EntityMessage::Move) << px << py << sf::Int8(Network::PlayerUpdateDelim);
    packet << sf::Int8(EntityMessage::Attack) << pa << sf::Int8(Network::PlayerUpdateDelim);
    client->Send(packet);
    outgoing.clear();
};

void S_Network::ClearSnapshots() { entitySnapshots.clear(); };

void S_Network::PerformInterpolation() {
    if (entitySnapshots.empty()) return;
    ClientEntityManager* entityMgr = (ClientEntityManager*)sysMgr->GetEntityManager();
    sf::Time t = client->GetTime();
    auto itr = ++entitySnapshots.begin();
    while (itr != entitySnapshots.end()) {
        if (entitySnapshots.begin()->first <= t.asMilliseconds() - NET_RENDER_DELAY &&
            itr->first >= t.asMilliseconds() - NET_RENDER_DELAY) {
                auto snap1 = entitySnapshots.begin();
                auto snap2 = itr;
                bool sortDrawables = false;
                for (auto snap = snap1->second.snapshots.begin(); snap != snap1->second.snapshots.end(); ) {
                    if (!entityMgr->HasEntity(snap->first)) {
                        if (entityMgr->AddEntity(snap->second.type, snap->first) == (int)Network::NullID) {
                            std::cout << "[S_Network] Failed adding entity type: " << snap->second.type << std::endl;
                            continue;
                        }
                        ApplyEntitySnapshot(snap->first, snap->second, true);
                        ++snap;
                        continue;
                    }
                    auto itr = snap2->second.snapshots.find(snap->first);
                    if (itr == snap2->second.snapshots.end()) {
                        sf::Lock lock(client->GetMutex());
                        entityMgr->RemoveEntity(snap->first);
                        snap = snap1->second.snapshots.erase(itr);
                        continue;
                    }
                    EntitySnapshot snapshot;
                    InterpolateSnapshot(snap->second, snap1->first, itr->second, snap2->first, 
                        snapshot, t.asMilliseconds() - NET_RENDER_DELAY);
                    ApplyEntitySnapshot(snap->first, snapshot, true);
                    if (!CompareSnapshots(snap->second, itr->second, true, false, false)) {
                        sortDrawables = true;
                    }
                    ++snap;
                }
                if (sortDrawables) sysMgr->GetSystem<S_Renderer>(System::Renderer)->SortDrawables();
                return;
            }
            entitySnapshots.erase(entitySnapshots.begin());
            itr = ++entitySnapshots.begin();
    }
};