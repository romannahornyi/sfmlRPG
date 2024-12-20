#pragma once
#include "C_TimedComponentBase.h"
#include <SFML/Graphics/Rect.hpp>

class C_Attacker : public C_TimedComponentBase {
public:
    C_Attacker() : C_TimedComponentBase(Component::Attacker), attackDuration(0), knockback(0.f), 
        attacked(false) {};

    void ReadIn(std::stringstream& _stream) {
        _stream >> offset.x >> offset.y >> attackArea.width >>
            attackArea.height >> knockback >> attackDuration;
    };

    void SetAreaPosition(const sf::Vector2f& _vec) {
        attackArea.left = _vec.x;
        attackArea.top = _vec.y;
    };

    float GetKnockback() { return knockback; };
    const sf::FloatRect& GetAreaOfAttack() { return attackArea; };
    bool HasAttacked() { return attacked; };
    void SetAttacked(bool& v) { attacked = v; };
    const sf::Vector2f& GetOffset() { return offset; };
    void SetOffset(const sf::Vector2f& _vec) { offset = _vec; };
private:
    sf::FloatRect attackArea;
    bool attacked;
    sf::Uint32 attackDuration;
    float knockback;
    sf::Vector2f offset;
};