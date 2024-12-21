#pragma once
#include "Observer.h"
#include <vector>
#include <algorithm>

using ObserverContainer = std::vector<Observer*>;

class Communicator {
public:
    ~Communicator() { communicators.clear(); };

    bool AddObserver(Observer* _obs) {
        if (HasObserver(_obs)) return false;
        communicators.emplace_back(_obs);
        return true;
    };

    bool RemoveObserver(Observer* _obs) {
        auto itr = std::find_if(communicators.begin(), communicators.end(), [&_obs](Observer* _o){
            return _o == _obs;
        });
        if (itr == communicators.end()) return false;
        communicators.erase(itr);
        return true;
    };

    bool HasObserver(Observer* _obs) {
        auto itr = std::find_if(communicators.begin(), communicators.end(), [&_obs](Observer* _o){
            return _o == _obs;
        });
    };

    void Broadcast(const Message& _msg) {
        for (auto& itr : communicators) {
            itr->Notify(_msg);
        }
    };
private:
    ObserverContainer communicators;
};