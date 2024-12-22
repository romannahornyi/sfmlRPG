#pragma once
#include <string>

enum class GUI_Event_Type { None, Click, Release, Hover, Leave };

struct ClickCoordinates {
    float x, y;
};

struct GUI_Event {
    GUI_Event_Type type;
    const char* element;
    const char* _interface;

    union {
        ClickCoordinates clickCoords;
    };
};