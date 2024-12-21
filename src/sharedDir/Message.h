#pragma once

using MessageType = unsigned int;

struct TwoFloats {
    float x, y;
};

struct Message {
    Message(const MessageType& _type) : type(_type) {};

    unsigned int sender;
    unsigned int receiver;
    MessageType type;
    union {
        TwoFloats _2f;
        int _int;
        bool _bool;
    };
};