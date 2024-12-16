#pragma once
#include "C_Base.h"
#include <SFML/Graphics/Rect.hpp>

enum class Origin { Top_Left, Abs_Center, Mid_Bottom };

class C_Collidable : public C_Base {
public:
    C_Collidable() : C_Base(Component::Collidable), origin(Origin::Mid_Bottom),
        collidingOnX(false), collidingOnY(false) {};
    
    void ReadIn(std::stringstream& _stream) {
        unsigned int _origin = 0;
        _stream >> AABB.width >> AABB.height >> offset.x >> offset.y >> _origin;
        origin = (Origin)_origin;
    };

    const sf::FloatRect& GetCollidable() { return AABB; };
    bool IsCollidingOnX() { return collidingOnX; };
    bool IsCollidingOnY() { return collidingOnY; };

    void CollideOnX() { collidingOnX = true; };
    void CollideOnY() { collidingOnY = true; };

    void ResetCollisionFlags() {
        collidingOnX = false;
        collidingOnY = false;
    };
    void SetCollidable(const sf::FloatRect& _rect) { AABB = _rect; };
    void SetOrigin(const Origin& _origin) { origin = _origin; };

    void SetSize(const sf::Vector2f& _vec) {
        AABB.width = _vec.x;
        AABB.height = _vec.y;
    };

    void SetPosition(const sf::Vector2f& _vec) {
        switch (origin) {
            case Origin::Top_Left:
            {
                AABB.left = _vec.x + offset.x;
                AABB.top = _vec.y + offset.y;
                break;
            }
            case Origin::Abs_Center:
            {
                AABB.left = _vec.x - (AABB.width / 2) + offset.x;
                AABB.top = _vec.y - (AABB.height / 2) + offset.y;
                break;
            }
            case Origin::Mid_Bottom:
            {
                AABB.left = _vec.x - (AABB.width / 2) + offset.x;
                AABB.top = _vec.y - AABB.height + offset.y;
                break;
            }
        }
    };
private:
    Origin origin;
    sf::Vector2f offset;
    sf::FloatRect AABB;

    bool collidingOnX;
    bool collidingOnY;
};