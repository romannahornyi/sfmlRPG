#include "EntitySnapshot.h"

sf::Packet& operator <<(sf::Packet& _packet, const EntitySnapshot& _snapshot) {
    _packet << _snapshot.type << _snapshot.name << _snapshot.position.x << _snapshot.position.y <<
        _snapshot.elevation << _snapshot.velocity.x << _snapshot.velocity.y << 
        _snapshot.acceleration.x << _snapshot.acceleration.y << _snapshot.direction <<
        _snapshot.state << _snapshot.health;
};

sf::Packet& operator >>(sf::Packet& _packet, EntitySnapshot& _snapshot) {
    _packet >> _snapshot.type >> _snapshot.name >> _snapshot.position.x >> _snapshot.position.y >>
        _snapshot.elevation >> _snapshot.velocity.x >> _snapshot.velocity.y >>
        _snapshot.acceleration.x >> _snapshot.acceleration.y >> _snapshot.direction >>
        _snapshot.state >> _snapshot.health;
};