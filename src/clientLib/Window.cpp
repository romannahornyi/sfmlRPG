#include "Window.h"

Window::Window() { Setup("Window", sf::Vector2u(640, 480)); };

Window::Window(const std::string& _title, const sf::Vector2u& _size) {
    Setup(_title, _size);
};

Window::~Window() { Close(); };

void Window::Setup(const std::string& _title, const sf::Vector2u& _size) {
    windowTitle = _title;
    windowSize = _size;

    isDone = false;
    isFullscreen = false;
    isFocused = false;

    evMgr.AddCallback(StateType(0), "Fullscreen_toggle", &Window::ToggleFullscreen, this);
    evMgr.AddCallback(StateType(0), "Window_close", &Window::Close, this);

    Create();
};

void Window::Create() {
    sf::Uint32 style = sf::Style::Default;
    if (isFullscreen) style = sf::Style::Fullscreen;
    window.create(sf::VideoMode(window.getSize().x, window.getSize().y, 32), windowTitle, style);
};

void Window::BeginDraw() { window.clear(sf::Color::Black); };
void Window::EndDraw() { window.display(); };

bool Window::IsDone() { return isDone; };
bool Window::IsFullscreen() { return isFullscreen; };
bool Window::IsFocused() { return isFocused; };

sf::RenderWindow* Window::GetRenderWindow() { return &window; };
sf::Vector2u Window::GetWindowSize() { return windowSize; };
EventManager* Window::GetEventManager() { return &evMgr; };
sf::FloatRect Window::GetViewSpace() {
    sf::Vector2f viewCenter(window.getView().getCenter());
    sf::Vector2f viewSize(window.getView().getSize());
    sf::Vector2f viewSizeHalf(viewSize.x / 2, viewSize.y / 2);
    sf::FloatRect viewSpace(viewCenter - viewSizeHalf, viewSize);
    return viewSpace;
};

void Window::ToggleFullscreen(EventDetails* details) {
    isFullscreen = !isFullscreen;
    window.close();
    Create();
};

void Window::Close(EventDetails* details) {
    isDone = true;
};

void Window::Update() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::LostFocus) {
            isFocused = false;
            evMgr.SetFocus(false);
        } else if (event.type == sf::Event::GainedFocus) {
            isFocused = true;
            evMgr.SetFocus(true);
        }
        evMgr.HandleEvent(event);
    }
    evMgr.Update();
};