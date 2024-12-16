#pragma once
#include "C_Base.h"
#include <SFML/Graphics.hpp>

class C_Drawable : public C_Base {
public:
    C_Drawable(const Component& _type) : C_Base(_type) {};
    virtual ~C_Drawable() {};
    virtual void UpdatePosition(const sf::Vector2f& _vec) = 0;
    virtual const sf::Vector2u& GetSize() = 0;
    virtual void Draw(sf::RenderWindow* _wind) = 0;
private:

};