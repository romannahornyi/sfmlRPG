#include "S_State.h"
#include "SystemManager.h"

S_State::S_State(SystemManager* _sysMgr) : S_Base(System::State, _sysMgr) {
    Bitmask req;
    req.TurnOnBit((unsigned int)Component::State);
    requiredComponents.push_back(req);

    sysMgr->GetMessageHandler()->Subscribe(EntityMessage::Move, this);
    sysMgr->GetMessageHandler()->Subscribe(EntityMessage::Hurt, this);
    sysMgr->GetMessageHandler()->Subscribe(EntityMessage::Attack, this);
    sysMgr->GetMessageHandler()->Subscribe(EntityMessage::Switch_State, this);
};

S_State::~S_State() {};

void S_State::Update(float _dt) {
    EntityManager* entityMgr = sysMgr->GetEntityManager();
    for (auto& itr : entities) {
        C_State* state = entityMgr->GetComponent<C_State>(itr, Component::State);
        if (state->GetState() != EntityState::Walking) continue;
        Message msg((MessageType)EntityMessage::Is_Moving);
        msg.receiver = itr;
        sysMgr->GetMessageHandler()->Dispatch(msg);
    }
};

void S_State::HandleEvent(const EntityID& _entity, const EntityEvent& _event) {
    switch (_event) {
        case EntityEvent::Began_Idle:
            ChangeState(_entity, EntityState::Idle, false);
            break;
    }
};

void S_State::Notify(const Message& _msg) {
    if (!HasEntity(_msg.receiver)) return;
    EntityManager* entityMgr = sysMgr->GetEntityManager();
    EntityMessage m = (EntityMessage)_msg.type;
    switch (m) {
        case EntityMessage::Move:
        {
            C_State* state = entityMgr->GetComponent<C_State>(_msg.receiver, Component::State);
            if (state->GetState() == EntityState::Attacking || state->GetState() == EntityState::Dying) return;
            
            EntityEvent e;
            if (_msg._int == (int)Direction::Left) {
                e = EntityEvent::Moving_Left;
            } else if (_msg._int == (int)Direction::Right) {
                e = EntityEvent::Moving_Right;
            } else if (_msg._int == (int)Direction::Up) {
                e = EntityEvent::Moving_Up;
            } else if (_msg._int == (int)Direction::Down) {
                e = EntityEvent::Moving_Down;
            }

            sysMgr->AddEvent(_msg.receiver, (EventID)e);
            ChangeState(_msg.receiver, EntityState::Walking, false);
            break;
        }
        case EntityMessage::Attack:
        {
            C_State* state = entityMgr->GetComponent<C_State>(_msg.receiver, Component::State);
            if (state->GetState() != EntityState::Dying && state->GetState() != EntityState::Hurt) {
                sysMgr->AddEvent(_msg.receiver, (EventID)EntityEvent::Began_Attacking);
                ChangeState(_msg.receiver, EntityState::Attacking, false);
            }
            break;
        }
        case EntityMessage::Hurt:
        {
            C_State* state = entityMgr->GetComponent<C_State>(_msg.receiver, Component::State);
            if (state->GetState() != EntityState::Dying) {
                ChangeState(_msg.receiver, EntityState::Hurt, false);
            }
            break;
        }
        case EntityMessage::Switch_State:
        {
            ChangeState(_msg.receiver, (EntityState)_msg._int, false);
            break;
        }
    }
};

EntityState S_State::GetState(const EntityID& _entity) {
    EntityManager* entityMgr = sysMgr->GetEntityManager();
    C_State* state = entityMgr->GetComponent<C_State>(_entity, Component::State);
    return state->GetState();
};

void S_State::ChangeState(const EntityID& _entity, const EntityState& _state, bool _force) {
    EntityManager* entityMgr = sysMgr->GetEntityManager();
    C_State* state = entityMgr->GetComponent<C_State>(_entity, Component::State);
    if (state->GetState() == _state) return;
    if (!_force && state->GetState() == EntityState::Dying) return;
    state->SetState(_state);
    Message msg((MessageType)EntityMessage::State_Changed);
    msg.receiver = _entity;
    msg._int = (int)_state;
    sysMgr->GetMessageHandler()->Dispatch(msg);
};