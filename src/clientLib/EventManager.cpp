#include "EventManager.h"
#include "StateManager.h"

EventManager::EventManager() : currentState(StateType(0)), hasFocus(true) {
    LoadBindings();
};

EventManager::~EventManager() {
    for (auto& itr : bindings) {
        delete itr.second;
    }
};

bool EventManager::AddBinding(Binding* _binding) {
    if (bindings.find(_binding->name) != bindings.end()) {
        return false;
    };
    return bindings.emplace(_binding->name, _binding).second;
};

bool EventManager::RemoveBinding(const std::string& _name) {
    auto itr = bindings.find(_name);
    if (itr == bindings.end()) return false;
    delete itr->second;
    bindings.erase(itr);
    return true;
};

void EventManager::SetCurrentState(const StateType& _state) {
    currentState = _state;
};

void EventManager::SetFocus(bool _focus) {
    hasFocus = _focus;
};

void EventManager::HandleEvent(sf::Event& _event) {
    for (auto& b_itr : bindings) {
        Binding* bind = b_itr.second;
        for (auto& e_itr : bind->events) {
            EventType sfmlEvent = (EventType)_event.type;
            if (e_itr.first == EventType::GUI_Click || e_itr.first == EventType::GUI_Release ||
                e_itr.first == EventType::GUI_Hover || e_itr.first == EventType::GUI_Leave) {
                continue;
            }
            if (sfmlEvent != e_itr.first) continue;
            if (sfmlEvent == EventType::KeyDown || sfmlEvent == EventType::KeyUp) {
                if (e_itr.second.code == _event.key.code) {
                    if (bind->details.keyCode != -1) {
                        bind->details.keyCode = e_itr.second.code;
                    }
                    ++(bind->c);
                    break;
                }
            } else if (sfmlEvent == EventType::MButtonDown || sfmlEvent == EventType::MButtonUp) {
                if (e_itr.second.code == _event.mouseButton.button) {
                    bind->details.mousePos.x = _event.mouseButton.x;
                    bind->details.mousePos.y = _event.mouseButton.y;
                    if (bind->details.keyCode != -1) {
                        bind->details.keyCode = e_itr.second.code;
                    }
                    break;
                }
            } else {
                if (sfmlEvent == EventType::MouseWheel) {
                    bind->details.mouseWheelDelta = _event.mouseWheel.delta;
                } else if (sfmlEvent == EventType::WindowResized) {
                    bind->details.size.x = _event.size.width;
                    bind->details.size.y = _event.size.height;
                } else if (sfmlEvent == EventType::TextEntered) {
                    bind->details.textEntered = _event.text.unicode;
                }
                ++(bind->c);
            }
        }
    }
};

void EventManager::HandleEvent(GUI_Event& _guiEvent) {
    for (auto& b_itr : bindings) {
        Binding* bind = b_itr.second;
        for (auto& e_itr : bind->events) {
            if (e_itr.first != EventType::GUI_Click && e_itr.first != EventType::GUI_Hover &&
                e_itr.first != EventType::GUI_Release && e_itr.first != EventType::GUI_Leave) {
                    continue;
            }
            if ((e_itr.first == EventType::GUI_Click && _guiEvent.type != GUI_Event_Type::Click) ||
                (e_itr.first == EventType::GUI_Release && _guiEvent.type != GUI_Event_Type::Release) ||
                (e_itr.first == EventType::GUI_Hover && _guiEvent.type != GUI_Event_Type::Hover) ||
                (e_itr.first == EventType::GUI_Leave && _guiEvent.type != GUI_Event_Type::Leave)) {
                    continue;
            }
            if (strcmp(e_itr.second.gui._interface, _guiEvent._interface) ||
                strcmp(e_itr.second.gui.element, _guiEvent.element)) {
                    continue;
            }
            bind->details.guiInterface = e_itr.second.gui._interface;
            bind->details.guiElement = e_itr.second.gui.element;
            ++(bind->c); 
        }
    }
};

void EventManager::Update() {
    if (!hasFocus) return;
    for (auto& b_itr : bindings) {
        Binding* bind = b_itr.second;
        for (auto& e_itr : bind->events) {
            switch (e_itr.first) {
                case EventType::Keyboard:
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(e_itr.second.code))) {
                        if (bind->details.keyCode != -1) {
                            bind->details.keyCode = e_itr.second.code;
                        }
                        ++(bind->c);
                    }
                    break;
                }
                case EventType::Mouse:
                {
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button(e_itr.second.code))) {
                        if (bind->details.keyCode != -1) {
                            bind->details.keyCode = e_itr.second.code;
                        }
                        ++(bind->c);
                    }
                    break;
                }
                case EventType::Joystick:
                    break;
            }
            if (bind->events.size() == bind->c) {
                auto stateCallbacks = callbacks.find(currentState);
                auto otherCallbacks = callbacks.find(StateType(0));
                
                if (stateCallbacks != callbacks.end()) {
                    auto callItr = stateCallbacks->second.find(bind->name);
                    if (callItr != stateCallbacks->second.end()) {
                        callItr->second(&bind->details);
                    }
                }
                if (otherCallbacks != callbacks.end()) {
                    auto callItr = otherCallbacks->second.find(bind->name);
                    if (callItr != otherCallbacks->second.end()) {
                        callItr->second(&bind->details);
                    }
                }
            }
        }
        bind->c = 0;
        bind->details.Clear();
    }
};

void EventManager::LoadBindings() {
    std::string delimeter = ":";
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + "keys.cfg");
    if (!file.is_open()) {
        std::cerr << "[EventManager] Failed to open file: keys.cfg" << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        std::string callbackName;
        keystream >> callbackName;
        Binding* bind = new Binding(callbackName);
        while (!keystream.eof()) {
            std::string keyval;
            keystream >> keyval;
            int start = 0;
            int end = keyval.find(delimeter);
            if (end = std::string::npos) {
                delete bind;
                bind = nullptr;
                break;
            }
            EventType type = EventType(stoi(keyval.substr(start, end - start)));
            EventInfo eventInfo;
            if (type == EventType::GUI_Click || type == EventType::GUI_Release ||
                type == EventType::GUI_Hover || type == EventType::GUI_Leave) {
                start = end + delimeter.length();
                end = keyval.find(delimeter, start);
                std::string window = keyval.substr(start, end - start);
                std::string element;
                if (end != std::string::npos) {
                    start = end + delimeter.length();
                    end = keyval.length();
                    element = keyval.substr(start, end);
                }
                char* w = new char[window.length() + 1];
                char* e = new char[element.length() + 1];
                strcpy_s(w, window.length() + 1, window.c_str());
                strcpy_s(e, element.length() + 1, element.c_str());
                eventInfo.gui._interface = w;
                eventInfo.gui.element = e;
            } else {
                int code = stoi(keyval.substr(end + delimeter.length(),
                    keyval.find(delimeter, end + delimeter.length())));
                eventInfo.code = code;
            }
            bind->BindEvent(type, eventInfo);
        }
        if (!AddBinding(bind)) delete bind;
        bind = nullptr;
    }
    file.close();
};