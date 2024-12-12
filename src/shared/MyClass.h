#pragma once
#include <SFML/Graphics.hpp>
class MyClass {
    public:
        MyClass(int x) {
            this->value = x;
        }
        int getValue() const {
            return this->value;
        }
    private:
        int value;
};
