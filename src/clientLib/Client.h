#pragma once
#include <SFML/Network.hpp>
#include "PacketTypes.h"
#include "NetworkDefinitions.h"
#include <functional>
#include <iostream>

#define CLIENT_TIMEOUT 5000

class Client;
using PacketHandler = std::function<void(const PacketID&, sf::Packet&, Client*)>;

class Client {
public:
    Client();
    ~Client();

    bool Connect();
    bool Disconnect();
    
    void Listen();
    bool Send(sf::Packet& _p);
    const sf::Time& GetTime() const;
    
    const sf::Time& GetLastHeartbeat() const;
    void SetTime(const sf::Time& _t);
    void SetServerInformation(const sf::IpAddress& _ip, const PortNumber& _port);
    
    template <class T>
    void Setup(void(T::*handler)(const PacketID&, sf::Packet&, Client*), T* instance) {
        packetHandler = std::bind(handler, instance, std::placeholders::_1, 
            std::placeholders::_2, std::placeholders::_3);
    };
    void Setup(void(*handler)(const PacketID&, sf::Packet&, Client*));
    
    void UnregisterPacketHandler();
    void Update(const sf::Time& _t);
    
    bool IsConnected() const;
    void SetPlayerName(const std::string& _name);
    sf::Mutex& GetMutex();
private:
    std::string playerName;
    
    sf::UdpSocket socket;
    sf::IpAddress serverIp;
    PortNumber serverPort;
    PacketHandler packetHandler;
    bool connected;
    
    sf::Time serverTime;
    sf::Time lastHeartbeat;
    
    sf::Thread listenThread;
    sf::Mutex mutex;
};