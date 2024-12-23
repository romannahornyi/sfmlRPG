#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "EventManager.h"

class Window {
public:
    Window();
    Window(const std::string& _title, const sf::Vector2u& _size);
    ~Window();

    void BeginDraw();
    void EndDraw();

    void Update();

    bool IsDone();
    bool IsFullscreen();
    bool IsFocused();

    void ToggleFullscreen(EventDetails* details);
    void Close(EventDetails* details = nullptr);
    
    sf::RenderWindow* GetRenderWindow();
    EventManager* GetEventManager();
    sf::Vector2u GetWindowSize();
    sf::FloatRect GetViewSpace();
private:
    void Setup(const std::string& _title, const sf::Vector2u& _size);
    void Create();

    sf::RenderWindow window;
    EventManager evMgr;
    sf::Vector2u windowSize;
    std::string windowTitle;
    bool isDone;
    bool isFullscreen;
    bool isFocused;
};