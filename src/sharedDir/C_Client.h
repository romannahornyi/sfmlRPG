#pragma once
#include "C_Base.h"
#include "NetworkDefinitions.h"

class C_Client : public C_Base {
public:
    C_Client() : C_Base(Component::Client), clientID((ClientID)Network::NullID) {};
    void ReadIn(std::stringstream& _stream) {};

    ClientID GetClientID() { return clientID; };
    void SetClientID(const ClientID& _client) { clientID = _client; };
private:
    ClientID clientID;
};