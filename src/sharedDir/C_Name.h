#pragma once
#include "C_Base.h"
#include <string>

class C_Name : public C_Base {
public:
    C_Name() : C_Base(Component::Name) {};
    void ReadIn(std::stringstream& _stream) { _stream >> name; };
    
    void SetName(const std::string& _name) { name = _name; };
    const std::string& GetName() { return name; };
private:
    std::string name;
};