#include <iostream>
#include <SFML/Network.hpp>
#include "Server.h"

void Handler(sf::IpAddress& _ip, const PortNumber& _port, const PacketID& _pid, sf::Packet& _packet, Server* _server) {
    ClientID id = _server->GetClientID(_ip, _port);
    if (id >= 0) {
        if ((PacketType)_pid == PacketType::Disconnect) {
            _server->RemoveClient(_ip, _port);
            sf::Packet p;
            StampPacket(PacketType::Message, p);
            std::string message;
            message = "Client left! " + _ip.toString() + ":" + std::to_string(_port);
            p << message;
            _server->Broadcast(p, id);
        } else if ((PacketType)_pid == PacketType::Message) {
            std::string receivedMessage;
            _packet >> receivedMessage;
            std::string message = _ip.toString() + ":" + std::to_string(_port) + " :" + receivedMessage;
            sf::Packet p;
            StampPacket(PacketType::Message, p);
            p << message;
            _server->Broadcast(p, id);
        }
    } else {
        if ((PacketType)_pid == PacketType::Connect) {
            ClientID id = _server->AddClient(_ip, _port);
            sf::Packet p;
            StampPacket(PacketType::Connect, p);
            _server->Send(id, p);
        }
    }
};

void CommandProcess(Server* _server) {
    while (_server->IsRunning()) {
        std::string str;
        std::getline(std::cin, str);
        if (str == "!quit") {
            _server->Stop();
            break;
        } else if (str == "dc") {
            _server->DisconnectAll();
            std::cout << "...Disconnecting all clients." << std::endl;
        } else if (str == "list") {
            std::cout << _server->GetClientCount() << " clients online" << std::endl;
            std::cout << _server->GetClientList() << std::endl;
        }
    }
};

int main() {
    Server server(Handler);
    if (server.Start()) {
        sf::Thread c(&CommandProcess, &server);
        c.launch();
        sf::Clock clock;
        clock.restart();
        while (server.IsRunning()) {
            server.Update(clock.restart());
        }
        std::cout << "Stopping server..." << std::endl;
    } else {
        std::cout << "Server is not running" << std::endl;
    }
    return 0;
}