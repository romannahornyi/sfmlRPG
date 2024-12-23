#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <functional>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>
#include "Utilities.h"
#include "GUI_Event.h"

enum class EventType {
    KeyDown = sf::Event::KeyPressed, 
    KeyUp = sf::Event::KeyReleased,
    MButtonDown = sf::Event::MouseButtonPressed,
    MButtonUp = sf::Event::MouseButtonReleased,
    MouseWheel = sf::Event::MouseWheelMoved,
    WindowResized = sf::Event::Resized,
    GainedFocus = sf::Event::GainedFocus,
    LostFocus = sf::Event::LostFocus,
    MouseEntered = sf::Event::MouseEntered,
    MouseLeft = sf::Event::MouseLeft,
    Closed = sf::Event::Closed,
    TextEntered = sf::Event::TextEntered,
    Keyboard = sf::Event::Count + 1, Mouse, Joystick,
    GUI_Click, GUI_Release, GUI_Hover, GUI_Leave
};

struct EventInfo {
    EventInfo() { code = 0; };
    EventInfo(int _c) { code = _c; };
    EventInfo(const GUI_Event& _gui) { gui = _gui; };

    union {
        int code;
        GUI_Event gui;
    };
};

struct EventDetails {
    EventDetails(const std::string& _bindName) : name(_bindName) { Clear(); };

    std::string name;

    sf::Vector2i size;
    sf::Vector2i mousePos;

    sf::Uint32 textEntered;
    int keyCode;
    int mouseWheelDelta;

    std::string guiElement;
    std::string guiInterface;
    GUI_Event_Type guiEvent;

    void Clear() {
        size = sf::Vector2i(0, 0);
        mousePos = sf::Vector2i(0, 0);
        textEntered = 0;
        keyCode = -1;
        mouseWheelDelta = 0;
        guiElement = "";
        guiInterface = "";
        guiEvent = GUI_Event_Type::None;
    };
};

using Events = std::unordered_map<EventType, EventInfo>;

struct Binding {
    Binding(const std::string& _bindName) : name(_bindName), details(name), c(0) {};

    ~Binding() {
        for (auto itr = events.begin(); itr != events.end(); ++itr) {
            if (itr->first == EventType::GUI_Click || itr->first == EventType::GUI_Release ||
                itr->first == EventType::GUI_Hover || itr->first == EventType::GUI_Leave) {
                    delete [] itr->second.gui._interface;
                    delete [] itr->second.gui.element;
                }
        }
    };

    void BindEvent(const EventType& _type, EventInfo _info = EventInfo()) {
        events.emplace(_type, _info);
    };

    std::string name;
    EventDetails details;
    int c;
    Events events;
};

using Bindings = std::unordered_map<std::string, Binding*>;
using CallbackContainer = std::unordered_map<std::string, std::function<void(EventDetails*)>>;
enum class StateType;
using Callbacks = std::unordered_map<StateType, CallbackContainer>;

class EventManager {
public:
    EventManager();
    ~EventManager();

    bool AddBinding(Binding* _binding);
    bool RemoveBinding(const std::string& _name);

    void SetCurrentState(const StateType& _state);
    void SetFocus(bool _focus);

    template <class T>
    bool AddCallback(StateType _state, const std::string& _name, 
        void(T::*_func)(EventDetails*), T* _instance) {
        auto itr = callbacks[_state].emplace(_name, CallbackContainer()).first;
        auto callback = std::bind(_func, _instance, std::placeholders::_1);
        return itr->second.emplace(_name, callback).second;
    };

    bool RemoveCallback(StateType _state, const std::string& _name) {
        auto itr1 = callbacks.find(_state);
        if (itr1 == callbacks.end()) return false;
        auto itr2 = itr1->second.find(_name);
        if (itr2 == itr1->second.end()) return false;
        itr1->second.erase(itr2);
        return true;
    };

    void HandleEvent(sf::Event& _event);
    void HandleEvent(GUI_Event& _guiEvent);
    void Update();

    sf::Vector2i GetMousePosition(sf::RenderWindow* _wind = nullptr) {
        return (_wind ? sf::Mouse::getPosition(*_wind) : sf::Mouse::getPosition());
    };
private:
    void LoadBindings();

    Bindings bindings;
    Callbacks callbacks;
    StateType currentState;

    bool hasFocus;
};