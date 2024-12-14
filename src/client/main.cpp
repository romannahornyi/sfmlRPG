#include <iostream>
#include <SFML/Network.hpp>
#include "Client.h"

void HandlePacket(const PacketID& _pid, sf::Packet& _packet, Client* _client) {
    if ((PacketType)_pid == PacketType::Message) {
        std::string message;
        _packet >> message;
        std::cout << message;
    } else if ((PacketType)_pid == PacketType::Disconnect) {
        _client->Disconnect();
    }
};

void CommandProcess(Client* _client) {
    while (_client->IsConnected()) {
        std::string str;
        std::getline(std::cin, str);
        if (str != "") {
            if (str == "!quit") {
                _client->Disconnect();
                break;
            }
            sf::Packet p;
            StampPacket(PacketType::Message, p);
            p << str;
            _client->Send(p);
        }
    }
};

void main(int argc, char** argv) {
    sf::IpAddress ip;
    PortNumber port;
    if (argc == 1) {
        std::cout << "Enter Server IP: ";
        std::cin >> ip;
        std::cout << "Enter Server PORT: ";
        std::cin >> port;
    } else if (argc == 3) {
        ip = argv[1];
        port = atoi(argv[2]);
    } else {
        return;
    }
    Client client;
    client.SetServerInformation(ip, port);
    client.Setup(&HandlePacket);
    sf::Thread c(&CommandProcess);
    if (client.Connect()) {
        c.launch();
        sf::Clock clock;
        clock.restart();
        while (client.IsConnected()) {
            client.Update(clock.restart());
        }
    } else {
        std::cout << "Failed to connect." << std::endl;
    }
    std::cout << "Quitiing..." << std::endl;
};