#pragma once
#include <SFML/Network.hpp>
#include <SFML/System/Time.hpp>
#include <unordered_map>
#include <functional>
#include <iostream>
#include "PacketTypes.h"
#include "NetworkDefinitions.h"

#define HEARTBEAT_INTERVAL 1000
#define HEARTBEAT_RETRIES 5

struct ClientInfo {
    sf::IpAddress clientIP;
    PortNumber clientPORT;
    sf::Time lastHeartbeat;
    sf::Time heartbeatSent;
    bool heartbeatWaiting;
    unsigned short heartbeatRetry;
    unsigned int ping;
    ClientInfo(const sf::IpAddress& _ip, const PortNumber& _port, const sf::Time& _heartbeat) :
        clientIP(_ip), clientPORT(_port), lastHeartbeat(_heartbeat), heartbeatWaiting(false), heartbeatRetry(0),
        ping(0) {};
    ClientInfo& operator =(const ClientInfo& other) {
        clientIP = other.clientIP;
        clientPORT = other.clientPORT;
        lastHeartbeat = other.lastHeartbeat;
        heartbeatSent = other.heartbeatSent;
        heartbeatWaiting = other.heartbeatWaiting;
        heartbeatRetry = other.heartbeatRetry;
        ping = other.ping;
        return *this;
    };
};

using Clients = std::unordered_map<ClientID, ClientInfo>;
class Server;
using PacketHandler = std::function<void(sf::IpAddress&, const PortNumber&, const PacketID&, sf::Packet&, Server*)>;
using TimeoutHandler = std::function<void(const ClientID&)>;

class Server {
public:
    template <class T>
    Server(void(T::*handler)(sf::IpAddress&, const PortNumber&, const PacketID&, sf::Packet&, Server*), T* instance) :
        listenThread(&Server::Listen, this) {
        packetHandler = std::bind(handler, instance, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
            std::placeholders::_5);
    };
    Server(void(*handler)(sf::IpAddress&, const PortNumber&, const PacketID&, sf::Packet&, Server*));
    ~Server();

    template <class T>
    void BindTimeoutHandler(void(T::*handler)(const ClientID&), T* instance) {
        timeoutHandler = std::bind(handler, instance, std::placeholders::_1);
    };
    void BindTimeoutHandler(void(*handler)(const ClientID&));

    bool Send(const ClientID& _client, sf::Packet& _packet);
    bool Send(sf::IpAddress& _ip, const PortNumber& _port, sf::Packet& _packet);
    void Broadcast(sf::Packet& _packet, const ClientID& ignore = (ClientID)Network::NullID);

    void Listen();
    void Update(const sf::Time& _t);

    ClientID AddClient(const sf::IpAddress& _ip, const PortNumber& _port);
    ClientID GetClientID(const sf::IpAddress& _ip, const PortNumber& _port);
    bool HasClient(const ClientID& _client);
    bool HasClient(const sf::IpAddress& _ip, const PortNumber& _port);
    bool GetClientInfo(const ClientID& _client, ClientInfo& _info);
    bool RemoveClient(const ClientID& _client);
    bool RemoveClient(const sf::IpAddress& _ip, const PortNumber& _port);

    void DisconnectAll();
    bool Start();
    bool Stop();
    bool IsRunning();

    unsigned int GetClientCount();
    std::string GetClientList();

    sf::Mutex& GetMutex();
private:
    void Setup();
    ClientID lastID;

    sf::UdpSocket incoming;
    sf::UdpSocket outgoing;

    PacketHandler packetHandler;
    TimeoutHandler timeoutHandler;

    Clients clients;
    sf::Time serverTime;
    bool running;

    sf::Thread listenThread;
    sf::Mutex mutex;

    std::size_t totalSent;
    std::size_t totalReceived;
};