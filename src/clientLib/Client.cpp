#include "Client.h"

Client::Client() : listenThread(&Client::Listen, this) {};

Client::~Client() {
    socket.unbind();
};

bool Client::Connect() {
    if (connected) return false;
    socket.bind(sf::Socket::AnyPort);
    sf::Packet p;
    StampPacket(PacketType::Connect, p);
    p << playerName;
    if (socket.send(p, serverIp, serverPort) != sf::Socket::Done) {
        socket.unbind();
        return false;
    }
    socket.setBlocking(false);
    p.clear();
    sf::IpAddress recvIP;
    PortNumber recvPORT;
    sf::Clock timer;
    timer.restart();
    std::cout << "Attempting to connect to: " << serverIp << ":" << serverPort << std::endl;
    while (timer.getElapsedTime().asMilliseconds() < CLIENT_TIMEOUT) {
        sf::Socket::Status s = socket.receive(p, recvIP, recvPORT);
        if (s != sf::Socket::Done) continue;
        if (recvIP != serverIp) continue;
        PacketID id;
        if (!(p >> id)) break;
        if ((PacketType)id != PacketType::Connect) continue;
        packetHandler(id, p, this);
        connected = true;
        socket.setBlocking(true);
        lastHeartbeat = serverTime;
        listenThread.launch();
        return true;
    }
    std::cout << "Connection attempt failed. Server info: IP:" << serverIp << " PORT:" << serverPort << std::endl;
    socket.unbind();
    socket.setBlocking(true);
    return false;
};

void Client::Listen() {
    sf::Packet packet;
    sf::IpAddress recvIP;
    PortNumber recvPORT;
    while (connected) {
        packet.clear();
        sf::Socket::Status s = socket.receive(packet, recvIP, recvPORT);
        if (s != sf::Socket::Done) {
            if (connected) {
                std::cout << "Failed receiving a packet from: IP:" << recvIP <<
                    " PORT:" << recvPORT << ". Status: " << s << std::endl;
                continue;
            } else {
                std::cout << "Socket unbound." << std::endl;
                break;
            }
        }
        if (recvIP != serverIp) {
            //Ignore packets not sent from the server
            continue;
        }
        PacketID pid;
        if (!(packet >> pid)) {
            continue;
        }
        PacketType id = (PacketType)pid;
        if (id < PacketType::Disconnect || id >= PacketType::OutOfBounds) {
            continue;
        }
        if (id == PacketType::Heartbeat) {
            sf::Packet p;
            StampPacket(PacketType::Heartbeat, p);
            if (socket.send(p, serverIp, serverPort) != sf::Socket::Done) {
                std::cout << "Failed sending a heartbeat" << std::endl;
            } 
            sf::Int32 timestamp;
            p >> timestamp;
            SetTime(sf::milliseconds(timestamp));
            lastHeartbeat = serverTime;
        } else if (packetHandler) {
            packetHandler((PacketID)id, packet, this);
        }
    }
    std::cout << "...Listening stopped." << std::endl;
};  

bool Client::Disconnect() {
    if (!connected) return false;
    sf::Packet p;
    StampPacket(PacketType::Disconnect, p);
    sf::Socket::Status s = socket.send(p, serverIp, serverPort);
    socket.unbind();
    connected = false;
    if (s != sf::Socket::Done) return false;
    return true;
}

bool Client::Send(sf::Packet& _p) {
    if (!connected) return false;
    if (socket.send(_p, serverIp, serverPort) != sf::Socket::Done) {
        return false;
    }
    return true;
};

void Client::Setup(void(*handler)(const PacketID&, sf::Packet&, Client*)) {
    packetHandler = std::bind(handler, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
};

void Client::UnregisterPacketHandler() {
    packetHandler = nullptr;
};

void Client::Update(const sf::Time& _t) {
    if (!connected) return;
    if (serverTime.asMilliseconds() < 0) {
        serverTime -= sf::milliseconds(sf::Int32(Network::HighestTimestamp));
        return;
    }
    if (serverTime.asMilliseconds() >= CLIENT_TIMEOUT) {
        std::cout << "Server connection timed out" << std::endl;
        Disconnect();
    }
};

const sf::Time& Client::GetTime() const {
    return serverTime;
};

const sf::Time& Client::GetLastHeartbeat() const {
    return lastHeartbeat;
};

void Client::SetTime(const sf::Time& _t) {
    serverTime = _t;
};

void Client::SetServerInformation(const sf::IpAddress& _ip, const PortNumber& _port) {
    serverIp = _ip;
    serverPort = _port;
};

bool Client::IsConnected() const {
    return connected;
};

void Client::SetPlayerName(const std::string& _name) {
    playerName = _name;
};

sf::Mutex& Client::GetMutex() {
    return mutex;
};