#pragma once
#include "Message.h"

class Observer {
public:
    virtual ~Observer() {};
    virtual void Notify(const Message& _msg) = 0;
private:

};