#pragma once
#include <SFML/Network/Packet.hpp>
using PacketID = sf::Int8;
enum class PacketType {
    Disconnect = -1, Connect, Heartbeat, Snapshot,
    Player_Update, Message, Hurt, OutOfBounds
};
void StampPacket(const PacketType&, sf::Packet&);