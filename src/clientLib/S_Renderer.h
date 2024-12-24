#pragma once
#include <SFML/Graphics.hpp>
#include "S_Base.h"
#include "Window.h"
#include <algorithm>
#include "Directions.h"
#include "C_Position.h"
#include "C_Drawable.h"

class S_Renderer : public S_Base {
public:
    S_Renderer(SystemManager* _sysMgr);
    ~S_Renderer();

    void Update(float _dt);
    void HandleEvent(const EntityID& _entity, const EntityEvent& _event);
    void Notify(const Message& _msg);
    void Render(Window* _wind, unsigned int _layer);

    void SortDrawables();
private:
    void SetSheetDirection(const EntityID& _entity, const Direction& _dir);
};