#pragma once
#include "C_TimedComponentBase.h"

using Health = unsigned int;

class C_Health : public C_TimedComponentBase {
public:
    C_Health() : C_TimedComponentBase(Component::Health), hurtDuration(0), 
        deathDuration(0) {};
    void ReadIn(std::stringstream& _stream) {
        _stream >> maxHealth >> hurtDuration >> deathDuration;
        health = maxHealth;
    };

    Health GetHealth() { return health; };
    Health GetMaxHealth() { return maxHealth; };
    void SetHealth(const Health& _health) { health = _health; };
    void ResetHealth() { health = maxHealth; };
    
    sf::Uint32 GetHurtDuration() { return hurtDuration; };
    sf::Uint32 GetDeathDuration() {return deathDuration; };
private:
    sf::Uint32 hurtDuration;
    sf::Uint32 deathDuration;

    Health health;
    Health maxHealth;
};