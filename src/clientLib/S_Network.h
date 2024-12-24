#pragma once
#include "S_Base.h"
#include "EntitySnapshot.h"
#include "ClientEntityManager.h"
#include "EntityMessages.h"
#include "Client.h"
#include "S_Movement.h"
#include "S_State.h"
#include <unordered_map>
#include <map>
#include "NetSettings.h"

template <class T>
inline T InterpolateSnapshot(const sf::Int32& T1, const sf::Int32& T2, const T& VAL1, const T& VAL2, const sf::Int32& TX) {
    return ((VAL2 - VAL1)/(T2 - T1) * (TX - T1)) + VAL1;
};

void InterpolateSnapshot(const EntitySnapshot& VAL1, const sf::Int32& T1, const EntitySnapshot& VAL2, const sf::Int32& T2,
    EntitySnapshot& TARGET, const sf::Int32& TX);

bool CompareSnapshots(const EntitySnapshot& VAL1, const EntitySnapshot& VAL2, bool position = true,     
    bool physics = true, bool state = true);

using SnapshotMap = std::unordered_map<EntityID, EntitySnapshot>;
struct SnapshotDetails {
    SnapshotMap snapshots;
};
using SnapshotContainer = std::map<sf::Int32, SnapshotDetails>;
using OutgoingMessages = std::unordered_map<EntityMessage, std::vector<Message>>;

class S_Network : public S_Base {
public:
    S_Network(SystemManager* _sysMgr);
    ~S_Network();

    void Update(float _dt);
    void HandleEvent(const EntityID& _entity, const EntityEvent& _event);
    void Notify(const Message& _msg);

    void SetClient(Client* _client);
    void SetPlayerID(const EntityID& _entity);

    void AddEntitySnapshot(const EntityID& _entity, 
        const sf::Int32& _timestamp, EntitySnapshot& _snapshot);
    void SendPlayerOutgoing();
    void ClearSnapshots();
private:
    void ApplyEntitySnapshot(const EntityID& _entity, const EntitySnapshot& _snapshot, bool applyPhysics);
    void PerformInterpolation();

    SnapshotContainer entitySnapshots;
    EntityID player;
    OutgoingMessages outgoing;
    Client* client;
    sf::Time playerUpdateTimer;
};