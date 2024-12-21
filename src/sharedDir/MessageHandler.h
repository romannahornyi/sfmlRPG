#pragma once
#include "Communicator.h"
#include "EntityMessages.h"
#include <unordered_map>

using Subscriptions = std::unordered_map<EntityMessage, Communicator>;

class MessageHandler {
public:
    bool AddObserver(const EntityMessage& _msg, Observer* _obs) {
        communicators[_msg].AddObserver(_obs);
    };

    bool RemoveObserver(const EntityMessage& _msg, Observer* _obs) {
        communicators[_msg].RemoveObserver(_obs);
    };

    void Dispatch(const Message& _msg) {
        auto itr = communicators.find((EntityMessage)_msg.type);
        if (itr == communicators.end()) return;
        itr->second.Broadcast(_msg);
    };
private:
    Subscriptions communicators;
};