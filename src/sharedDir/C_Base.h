#pragma once
#include <iostream>
#include <sstream>
#include "ECS_Types.h"

class C_Base {
public:
    C_Base(const Component& _type) : type(_type) {};
    virtual ~C_Base() {};
    Component GetType() { return type; };

    virtual void ReadIn(std::stringstream& _stream) = 0;
    friend std::stringstream& operator >>(std::stringstream& _stream, C_Base& _comp) {
        _comp.ReadIn(_stream);
        return _stream;
    };
private:
    Component type;
};