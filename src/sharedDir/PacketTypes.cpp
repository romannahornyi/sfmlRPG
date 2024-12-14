#include "PacketTypes.h"
void StampPacket(const PacketType& type, sf::Packet& p) {
    p << (PacketID)type;
};