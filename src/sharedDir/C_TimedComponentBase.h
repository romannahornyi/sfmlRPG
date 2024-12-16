#pragma once
#include "C_Base.h"
#include <SFML/System/Time.hpp>

class C_TimedComponentBase : public C_Base {
public:
    C_TimedComponentBase(const Component& _type) : C_Base(_type), duration(sf::milliseconds(0)) {};
    virtual ~C_TimedComponentBase() {};

    const sf::Time& GetTimer() const { return duration; };
    void SetTimer(const sf::Time& _time) { duration = _time; };
    void AddToTimer(const sf::Time& _time) { duration += _time; };

    void Reset() { duration = sf::milliseconds(0); };
private:
    sf::Time duration;
};