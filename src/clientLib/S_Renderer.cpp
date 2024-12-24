#include "S_Renderer.h"
#include "C_SpriteSheet.h"
#include "SystemManager.h"

S_Renderer::S_Renderer(SystemManager* _sysMgr) : S_Base(System::Renderer, _sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::SpriteSheet);
    req.TurnOnBit((unsigned int)Component::Position);
    requiredComponents.push_back(req);
    req.Clear();

    sysMgr->GetMessageHandler()->Subscribe(EntityMessage::Direction_Changed, this);
};

S_Renderer::~S_Renderer() {};

void S_Renderer::Update(float _dt) {
    EntityManager* entityMgr = sysMgr->GetEntityManager();
    for (auto& entity : entities) {
        C_Position* pos = entityMgr->GetComponent<C_Position>(entity, Component::Position);
        C_Drawable* draw = nullptr;
        if (entityMgr->HasComponent(entity, Component::SpriteSheet)) {
            draw = entityMgr->GetComponent<C_Drawable>(entity, Component::SpriteSheet);
        } else continue;
        draw->UpdatePosition(pos->GetPosition());
    }
};

void S_Renderer::HandleEvent(const EntityID& _entity, const EntityEvent& _event) {
    if (_event == EntityEvent::Moving_Down || _event == EntityEvent::Moving_Up ||
        _event == EntityEvent::Moving_Right || _event == EntityEvent::Moving_Left ||
        _event == EntityEvent::Elevation_Change || _event == EntityEvent::Spawned) {
            SortDrawables();
        }
};

void S_Renderer::Notify(const Message& _msg) {
    EntityMessage m = (EntityMessage)_msg.type;
    switch (m) {
    case EntityMessage::Direction_Changed:
        SetSheetDirection(_msg.receiver, (Direction)_msg._int);
        break;
    }
};

void S_Renderer::Render(Window* _wind, unsigned int _layer) {
    EntityManager* entityMgr = sysMgr->GetEntityManager();
    for (auto& entity : entities) {
        C_Position* pos = entityMgr->GetComponent<C_Position>(entity, Component::Position);
        if (pos->GetElevation() < _layer) continue;
        if (pos->GetElevation() > _layer) break;
        C_Drawable* draw = nullptr;
        if (!entityMgr->HasComponent(entity, Component::SpriteSheet)) continue;
        draw = entityMgr->GetComponent<C_Drawable>(entity, Component::SpriteSheet);
        sf::FloatRect drawableBounds;
        drawableBounds.left = pos->GetPosition().x - (draw->GetSize().x / 2.f);
        drawableBounds.top = pos->GetPosition().y - draw->GetSize().y;
        drawableBounds.width = draw->GetSize().x;
        drawableBounds.height = draw->GetSize().y;
        sf::FloatRect viewSpace = _wind->GetViewSpace();
        if (!viewSpace.intersects(drawableBounds)) continue;
        draw->Draw(_wind->GetRenderWindow());
    }
};

void S_Renderer::SetSheetDirection(const EntityID& _entity, const Direction& _dir) {
    EntityManager* entityMgr = sysMgr->GetEntityManager();
    if (!entityMgr->HasComponent(_entity, Component::SpriteSheet)) return;
    C_SpriteSheet* sheet = entityMgr->GetComponent<C_SpriteSheet>(_entity, Component::SpriteSheet);
    sheet->GetSpriteSheet()->SetDirection(_dir);
};

void S_Renderer::SortDrawables() {
    EntityManager* entityMgr = sysMgr->GetEntityManager();
    std::sort(entities.begin(), entities.end(), [entityMgr](unsigned int _f, unsigned int _s){
        auto p1 = entityMgr->GetComponent<C_Position>(_f, Component::Position);
        auto p2 = entityMgr->GetComponent<C_Position>(_s, Component::Position);
        if (p1->GetElevation() == p2->GetElevation()) {
            return p1->GetPosition().y < p2->GetPosition().y;
        }
        return p1->GetElevation() < p2->GetElevation();
    });
};