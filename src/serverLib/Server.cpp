#include "Server.h"

Server::Server(void(*handler)(sf::IpAddress&, const PortNumber&, const PacketID&, sf::Packet&, Server*)) :
    listenThread(&Server::Listen, this), running(false) {
    packetHandler = std::bind(handler, std::placeholders::_1, 
        std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
        std::placeholders::_5);
};

Server::~Server() {  
    Stop();
};

void Server::BindTimeoutHandler(void(*handler)(const ClientID&)) {
    timeoutHandler = std::bind(handler, std::placeholders::_1);
};

bool Server::Start() {
    if (running) return false;
    if (incoming.bind((sf::Int32)Network::ServerPort) != sf::Socket::Done) {
        return false;
    }
    outgoing.bind(sf::Socket::AnyPort);
    Setup();
    std::cout << "Incoming port: " << incoming.getLocalPort() << ", outgoing port: " <<
        outgoing.getLocalPort() << std::endl;
    listenThread.launch();
    running = true;
    return true;
};

bool Server::Stop() {
    if (!running) return false;
    DisconnectAll();
    running = false;
    incoming.unbind();
    return true;
};

void Server::Setup() {
    lastID = 0;
    running = false;
    totalSent = 0;
    totalReceived = 0;
};

bool Server::Send(const ClientID& _client, sf::Packet& _packet) {
    sf::Lock lock(mutex);
    auto itr = clients.find(_client);
    if (itr == clients.end()) return false;
    if (outgoing.send(_packet, itr->second.clientIP, itr->second.clientPORT) != sf::Socket::Done) {
        std::cout << "Error sending a packet." << std::endl;
        return false;
    }
    totalSent += _packet.getDataSize();
    return true;
};

bool Server::Send(sf::IpAddress& _ip, const PortNumber& _port, sf::Packet& _packet) {
    if (outgoing.send(_packet, _ip, _port) != sf::Socket::Done) {
        std::cout << "Error sending a packet." << std::endl;
        return false;
    }
    totalSent += _packet.getDataSize();
    return true;
};

void Server::Broadcast(sf::Packet& _packet, const ClientID& _ignore) {
    sf::Lock lock(mutex);
    for (auto& itr : clients) {
        if (itr.first != _ignore) {
            if (outgoing.send(_packet, itr.second.clientIP, itr.second.clientPORT) != 
                sf::Socket::Done) {
                std::cout << "Error broadcasting a packet to client: " << itr.first << std::endl;
                continue;
            }
            totalSent += _packet.getDataSize();
        }
    }
};

void Server::Listen() {
    sf::IpAddress ip;
    PortNumber port;
    sf::Packet packet;
    while (running) {
        packet.clear();
        sf::Socket::Status s = incoming.receive(packet, ip, port);
        if (s != sf::Socket::Done) {
            if (running) {
                std::cout << "Error receiving a packet from: " << ip << ":" << port <<
                    ". Code: " << s << std::endl;
                continue; 
            } else {
                std::cout << "Socket unbound." << std::endl;
                break;
            }
        }
        totalReceived += packet.getDataSize();
        PacketID pid;
        if (!(packet >> pid)) continue;
        PacketType id = (PacketType)pid;
        if (id < PacketType::Disconnect || id >= PacketType::OutOfBounds) continue;
        if (id == PacketType::Heartbeat) {
            sf::Lock lock(mutex);
            for (auto& itr : clients) {
                if (itr.second.clientIP != ip || itr.second.clientPORT != port) {
                    continue;
                }
                if (!itr.second.heartbeatWaiting) {
                    std::cout << "Invalid heartbeat packet received." << std::endl;
                    break;
                }
                itr.second.ping = serverTime.asMilliseconds() - itr.second.heartbeatSent.asMilliseconds();
                itr.second.heartbeatWaiting = false;
                itr.second.heartbeatRetry = 0;
                itr.second.lastHeartbeat = serverTime;
                break;
            }
        } else if (packetHandler) {
            packetHandler(ip, port, (PacketID)id, packet, this);
        }
    }
    std::cout << "...Listening stopped." << std::endl;
};

ClientID Server::AddClient(const sf::IpAddress& _ip, const PortNumber& _port) {
    sf::Lock lock(mutex);
    for (auto& itr : clients) {
        if (itr.second.clientIP == _ip && itr.second.clientPORT == _port) {
            return ClientID(Network::NullID);
        }
    }
    ClientID id = lastID;
    ClientInfo info(_ip, _port, serverTime);
    clients.insert(std::make_pair(id, info));
    ++lastID;
    return id;
};

ClientID Server::GetClientID(const sf::IpAddress& _ip, const PortNumber& _port) {
    sf::Lock lock(mutex);
    for (auto& itr : clients) {
        if (itr.second.clientIP == _ip && itr.second.clientPORT == _port) {
            return itr.first;
        }
    }
    return ClientID(Network::NullID);
};

bool Server::HasClient(const ClientID& _client) {
    return (clients.find(_client) != clients.end());
};

bool Server::HasClient(const sf::IpAddress& _ip, const PortNumber& _port) {
    return (GetClientID(_ip, _port) >= 0);
};

bool Server::IsRunning() {
    return running;
};

sf::Mutex& Server::GetMutex() {
    return mutex;
};

bool Server::GetClientInfo(const ClientID& _client, ClientInfo& _info) {
    sf::Lock lock(mutex);
    for (auto& itr : clients) {
        if (itr.first == _client) {
            _info = itr.second;
            return true;
        }
    }
    return false;
};

bool Server::RemoveClient(const ClientID& _client) {
    sf::Lock lock(mutex);
    auto itr = clients.find(_client);
    if (itr == clients.end()) return false;
    sf::Packet packet;
    StampPacket(PacketType::Disconnect, packet);
    packet << _client;
    Send(_client, packet);
    clients.erase(itr);
    return true;
};

bool Server::RemoveClient(const sf::IpAddress& _ip, const PortNumber& _port) {
    sf::Lock lock(mutex);
    for (auto itr = clients.begin(); itr != clients.end(); ++itr) {
        if (itr->second.clientIP == _ip && itr->second.clientPORT == _port) {
            sf::Packet packet;
            StampPacket(PacketType::Disconnect, packet);
            packet << itr->first;
            Send(itr->first, packet);
            clients.erase(itr);
            return true;
        }
    }
    return false;
};

void Server::DisconnectAll() {
    if (!running) return;
    sf::Packet p;
    StampPacket(PacketType::Disconnect, p);
    Broadcast(p);
    sf::Lock lock(mutex);
    clients.clear();
};

void Server::Update(const sf::Time& _time) {
    serverTime += _time;
    if (serverTime.asMilliseconds() < 0) {
        serverTime -= sf::milliseconds(sf::Int32(Network::HighestTimestamp));
        sf::Lock lock(mutex);
        for (auto& itr : clients) {
            itr.second.lastHeartbeat = sf::milliseconds(
                std::abs(itr.second.lastHeartbeat.asMilliseconds() - sf::Int32(Network::HighestTimestamp))
            );
        }
    }
    sf::Lock lock(mutex);
    for (auto itr = clients.begin(); itr != clients.end(); ) {
        sf::Int32 elapsed = serverTime.asMilliseconds() - itr->second.lastHeartbeat.asMilliseconds();
        if (elapsed >= HEARTBEAT_INTERVAL) {
            if (elapsed >= sf::Int32(Network::ClientTimeout) || itr->second.heartbeatRetry >= HEARTBEAT_RETRIES) {
                std::cout << "Client " << itr->first << " has timed out." << std::endl;
                if (timeoutHandler) timeoutHandler(itr->first);
                itr = clients.erase(itr);
            }
            if (!itr->second.heartbeatWaiting || elapsed >= (HEARTBEAT_INTERVAL * (itr->second.heartbeatRetry + 1))) {
                //Heartbeat
                if (itr->second.heartbeatRetry >= 3) {
                    std::cout << "Retry(" << itr->second.heartbeatRetry << ") heartbeat for client " << itr->first << std::endl;
                }
                sf::Packet heartbeat;
                StampPacket(PacketType::Heartbeat, heartbeat);
                heartbeat << serverTime.asMilliseconds();
                Send(itr->first, heartbeat);
                if (itr->second.heartbeatRetry == 0) {
                    itr->second.heartbeatSent = serverTime;
                }
                itr->second.heartbeatWaiting = true;
                ++itr->second.heartbeatRetry;
                totalSent += heartbeat.getDataSize();
            }
        }
        ++itr;
    }
};

unsigned int Server::GetClientCount() {
    return (unsigned int)clients.size();
};

std::string Server::GetClientList() {
    std::string list;
    std::string delimeter = "-------------------------";
    list = delimeter;
    list += '\n';
    list += "ID";
    list += '\t';
    list += "Client IP:PORT";
    list += '\t'; list += '\t';
    list += "Ping";
    list += '\n';
    list += delimeter;
    list += '\n';
    for (auto itr = clients.begin(); itr != clients.end(); ++itr) {
        list += std::to_string(itr->first);
        list += '\t';
        list += itr->second.clientIP.toString() + ":" + std::to_string(itr->second.clientPORT);
        list += '\t';
        list += std::to_string(itr->second.ping) + " ms.";
        list += '\n';
    }
    list += delimeter;
    list += '\n';
    list += "Total data sent: " + std::to_string(totalSent / 1000) + " kB. Total data received: " + 
        std::to_string(totalReceived / 1000) + " kB.";
    return list;
};