#ifndef MYCLASS_H
#define MYCLASS_H
#include <SFML/Graphics.hpp>
class MyClass {
    public:
        MyClass(int x);
        int getValue() const;
    private:
        int value;
};
#endif