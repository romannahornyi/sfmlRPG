#pragma once
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Vector2.hpp>

struct EntitySnapshot {
    std::string type;
    sf::Vector2f position;
    sf::Int32 elevation;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    sf::Uint8 direction;
    sf::Uint8 state;
    sf::Uint8 health;
    std::string name;
};

sf::Packet& operator <<(sf::Packet& _packet, const EntitySnapshot& _snapshot);
sf::Packet& operator >>(sf::Packet& _packet, EntitySnapshot& _snapshot);