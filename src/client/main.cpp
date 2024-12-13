#include <SFML/Graphics.hpp>
#include "MyClass.h"
#include "Zalupa.h"
#include <iostream>
int main() {
    MyClass item(5);
    Zalupa z(10);
    std::cout << item.getValue() << ' ' << z.getZalupa() << std::endl;
    sf::RenderWindow window(sf::VideoMode(640, 480), "SFML works!");
    sf::CircleShape shape(10.f);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear(sf::Color::Black);
        shape.setFillColor(sf::Color::Green);
        window.draw(shape);
        window.display();
    }   
    return 0;
}