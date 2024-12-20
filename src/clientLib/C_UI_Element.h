#pragma once
#include "C_Base.h"
#include <SFML/System/Vector2.hpp>

class C_UI_Element : C_Base {
public:
    C_UI_Element() : C_Base(Component::UI_Element), showHealth(false), showName(false) {};
    void ReadIn(std::stringstream& _stream) {
        _stream >> offset.x >> offset.y;
    };

    void SetOffset(const sf::Vector2f& _vec) { offset = _vec; };
    const sf::Vector2f& GetOffset() { return offset; };

    void SetShowHealth(bool _health) { showHealth = _health; };
    void SetShowName(bool _name) { showName = _name; };
    bool GetHealth() { return showHealth; };
    bool GetName() { return showName; };
private:
    sf::Vector2f offset;
    bool showHealth;
    bool showName;
};