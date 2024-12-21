#pragma once
#include <queue>

using EventID = unsigned int;

class EventQueue {
public:
    void AddEvent(const EventID& _event) { pq.push(_event); };

    bool ProcessEvents(EventID& _event) {
        if (pq.empty()) return false;
        _event = pq.front();
        pq.pop();
        return true;
    };

    void Clear() { 
        while (!pq.empty()) {
            pq.pop();
        }
    };
private:
    std::queue<EventID> pq;
};