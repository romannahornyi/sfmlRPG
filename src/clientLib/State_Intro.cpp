#include "State_Intro.h"
#include "SharedContext.h"

State_Intro::State_Intro(StateManager* _stateMgr) : BaseState(_stateMgr) {};

State_Intro::~State_Intro() {};

void State_Intro::OnCreate() {
    sf::Vector2u windSize = stateMgr->GetContext()->window->GetWindowSize();
    TextureManager* textMgr = stateMgr->GetContext()->textMgr;
    textMgr->RequireResource("Intro");
    introSprite.setTexture(*textMgr->GetResource("Intro"));
    introSprite.setOrigin(textMgr->GetResource("Intro")->getSize().x / 2.0f, 
        textMgr->GetResource("Intro")->getSize().y / 2.0f);
    text.setCharacterSize(15);
    text.setString(sf::String("press space to continue"));
    sf::FloatRect windRect = text.getLocalBounds();
    text.setOrigin(windRect.left + windRect.width / 2.0f, windRect.top + windRect.height / 2.0f);
    text.setPosition(introSprite.getPosition().x, introSprite.getPosition().y + 
        textMgr->GetResource("Intro")->getSize().y / 1.5f);
    EventManager* evMgr = stateMgr->GetContext()->evMgr;
    evMgr->AddCallback(StateType::Intro, "Intro_Continue", &State_Intro::Continue, this);
    stateMgr->GetContext()->soundMgr->PlayMusic("Electrix", 100.f, true);
};

void State_Intro::OnDestroy() {
    TextureManager* textMgr = stateMgr->GetContext()->textMgr;
    textMgr->ReleaseResource("Intro");
    EventManager* evMgr = stateMgr->GetContext()->evMgr;
    evMgr->RemoveCallback(StateType::Intro, "Intro_Continue");
};

void State_Intro::Draw() {
    sf::RenderWindow* wind = stateMgr->GetContext()->window->GetRenderWindow();
    wind->draw(introSprite);
    wind->draw(text);
};

void State_Intro::Continue(EventDetails* details) {
    stateMgr->SwitchTo(StateType::MainMenu);
    stateMgr->Remove(StateType::Intro);
};

void State_Intro::Update(const sf::Time& _time) {};
void State_Intro::Activate() {};
void State_Intro::Deactivate() {};