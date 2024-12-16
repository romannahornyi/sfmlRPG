#pragma once
#include <SFML/System/Vector2.hpp>
#include "C_Base.h"

class C_Position : public C_Base {
public:
    C_Position() : C_Base(Component::Position), elevation(0) {};
    ~C_Position() {};

    void ReadIn(std::stringstream& _stream) {
        _stream >> position.x >> position.y >> elevation;
    };

    const sf::Vector2f& GetPosition() { return position; };
    const sf::Vector2f& GetOldPosition() { return positionOld; };
    unsigned int GetElevation() { return elevation; };

    void SetPosition(const sf::Vector2f& _vec) {
        positionOld = position;
        position = _vec;
    };
    void SetPosition(float _x, float _y) {
        positionOld = position;
        position = sf::Vector2f(_x, _y);
    };

    void SetElevation(unsigned int _elevation) {
        elevation = _elevation;
    };
    
    void MoveBy(const sf::Vector2f& _vec) {
        positionOld = position;
        position += _vec;
    };
    void MoveBy(float _x, float _y) {
        positionOld = position;
        position += sf::Vector2f(_x, _y);
    };
private:
    sf::Vector2f position;
    sf::Vector2f positionOld;
    unsigned int elevation;
};