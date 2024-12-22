#pragma once
#include "C_Base.h"
#include <SFML/System/Vector2.hpp>
#include "Directions.h"

class C_Movable : public C_Base {
public:
    C_Movable() : C_Base(Component::Movable),
        velocityMax(0.f), direction((Direction)0) {};
    void ReadIn(std::stringstream& _stream) {
        _stream >> velocityMax >> speed.x >> speed.y;
        unsigned int dir = 0;
        _stream >> dir;
        direction = (Direction)dir;
    };

    void SetDirection(const Direction& _dir) { direction = _dir; };
    void SetVelocityMax(float _max) {velocityMax = _max; };
    void SetVelocity(const sf::Vector2f& _vec) { velocity = _vec; };
    void SetSpeed(const sf::Vector2f& _vec) { speed = _vec; };
    void SetAcceleration(const sf::Vector2f& _vec) { acceleration = _vec; };

    Direction GetDirection() { return direction; };
    float GetVelocityMax() { return velocityMax; };
    const sf::Vector2f& GetVelocity() { return velocity; };
    const sf::Vector2f& GetSpeed() { return speed; };
    const sf::Vector2f& GetAcceleration() { return acceleration; };

    void AddVelocity(const sf::Vector2f& _vec) {
        velocity += _vec;
        if (std::abs(velocity.x) > velocityMax) {
            velocity.x = velocityMax * (velocity.x / std::abs(velocity.x));
        }
        if (std::abs(velocity.y) > velocityMax) {
            velocity.y = velocityMax * (velocity.y / std::abs(velocity.y));
        }
    };

    void ApplyFriction(const sf::Vector2f& _vec) {
        if (velocity.x != 0 && _vec.x != 0) {
            if (std::abs(velocity.x) - std::abs(_vec.x) < 0) {
                velocity.x = 0;
            } else {
                velocity.x += (velocity.x > 0 ? (_vec.x * -1) : _vec.x);
            }
        }
        if (velocity.y != 0 && _vec.y != 0) {
            if (std::abs(velocity.y) - std::abs(_vec.y) < 0) {
                velocity.y = 0;
            } else {
                velocity.y += (velocity.y > 0 ? (_vec.y * -1) : _vec.y);
            }
        }
    };

    void Accelerate(const sf::Vector2f& _vec) { acceleration += _vec; };
    void Accelerate(float _x, float _y) { acceleration += sf::Vector2f(_x, _y); };

    void Move(const Direction& _dir) {
        if (_dir == Direction::Up) {
            acceleration.y -= speed.y;
        } else if (_dir == Direction::Down) {
            acceleration.y += speed.y;
        } else if (_dir == Direction::Left) {
            acceleration.x -= speed.x;
        } else if (_dir == Direction::Right) {
            acceleration.x += speed.x;
        }
    };
private:
    sf::Vector2f velocity;
    float velocityMax;
    sf::Vector2f speed;
    sf::Vector2f acceleration;
    Direction direction;
};