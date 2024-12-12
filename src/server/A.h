#ifndef A_H
#define A_H
#include <SFML/Graphics.hpp>
#include "MyClass.h"
class A {
    public:
        A(char x);
        char getValue() const;
    private:
        char value;
};
#endif