#include <SFML/Graphics.hpp>
#include "MyClass.h"
#include <iostream>
int main() {
    MyClass item(5);
    std::cout << item.getValue() << std::endl;
    sf::RenderWindow window(sf::VideoMode(640, 480), "Server");
    sf::CircleShape shape;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear(sf::Color::Black);
        shape.setFillColor(sf::Color::Red);
        window.draw(shape);
        window.display();
    }
    return 0;
}