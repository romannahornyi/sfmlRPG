#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(TextureManager* _textMgr) : textMgr(_textMgr), animCurrent(nullptr),
    spriteScale(1.f, 1.f), direction(Direction::Down) {};

SpriteSheet::~SpriteSheet() { ReleaseSheet(); };

void SpriteSheet::ReleaseSheet() {
    textMgr->ReleaseResource(texture);
    animCurrent = nullptr;
    while (animations.begin() != animations.end()) {
        delete animations.begin()->second;
        animations.erase(animations.begin());
    }
};

const sf::Vector2u& SpriteSheet::GetSpriteSize() const {
    return spriteSize;
};

const sf::Vector2f& SpriteSheet::GetSpritePosition() const {
    return sprite.getPosition();
};

void SpriteSheet::SetSpriteSize(const sf::Vector2u& _size) {
    spriteSize = _size;
};

void SpriteSheet::SetSpritePosition(const sf::Vector2f& _pos) {
    sprite.setPosition(_pos);
};

void SpriteSheet::CropSprite(const sf::IntRect& _rect) {
    sprite.setTextureRect(_rect);
};

void SpriteSheet::SetDirection(const Direction& _dir) {
    if (direction == _dir) return;
    direction = _dir;
    animCurrent->CropSprite();
};

Direction SpriteSheet::GetDirection() const {
    return direction; 
};

void SpriteSheet::SetSpriteSpacing(const sf::Vector2f& _vec) {
    spriteSpacing = _vec;
};

void SpriteSheet::SetSheetPadding(const sf::Vector2f& _vec) {
    sheetPadding = _vec;
};

const sf::Vector2f& SpriteSheet::GetSpriteSpacing() const {
    return spriteSpacing;
};

const sf::Vector2f& SpriteSheet::GetSheetPadding() const {
    return sheetPadding; 
};

Anim_Base* SpriteSheet::GetCurrentAnimation() {
    return animCurrent;
};

bool SpriteSheet::SetCurrentAnimation(const std::string& _name, bool play, bool loop) {
    auto itr = animations.find(_name);
    if (itr == animations.end()) return false;
    if (itr->second == animCurrent) return false;
    if (animCurrent) {
        animCurrent->Stop();
    }
    animCurrent = itr->second;
    animCurrent->SetLooping(loop);
    if (play) animCurrent->Play();
    animCurrent->CropSprite();
    return true;
};

void SpriteSheet::Update(float _dt) {
    animCurrent->Update(_dt);
};

void SpriteSheet::Draw(sf::RenderWindow* _wind) {
    _wind->draw(sprite);
};

bool SpriteSheet::LoadSheet(const std::string& _file) {
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + _file);
    if (!file.is_open()) {
        std::cout << "Failed to open sheet file: " << _file << std::endl;
        return false;
    }
    ReleaseSheet();
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "Texture") {
            if (texture != "") {
                std::cout << "Duplicate texture entries in file: " << _file << std::endl;
                continue;
            }
            std::string text;
            keystream >> text;
            if (!textMgr->RequireResource(text)) {
                std::cout << "Failed to load texture from file: " << text << std::endl;
                continue;
            }
            texture = text;
            sprite.setTexture(*textMgr->GetResource(texture));
        } else if (type == "Size") {
            keystream >> spriteSize.x >> spriteSize.y;
            SetSpriteSize(spriteSize);
        } else if (type == "Scale") {
            keystream >> spriteScale.x >> spriteScale.y;
            sprite.setScale(spriteScale);
        } else if (type == "Padding") {
            keystream >> sheetPadding.x >> sheetPadding.y;
        } else if (type == "Spacing") {
            keystream >> spriteSpacing.x >> spriteSpacing.y;
        } else if (type == "AnimationType") {
            keystream >> animType;
        } else if (type == "Animation") {
            std::string name;
            keystream >> name;
            if (animations.find(name) != animations.end()) {
                std::cout << "Duplicate animation name in file: " << _file << std::endl;
                continue;
            }
            
            Anim_Base* anim = nullptr;
            if (animType == "Directional") {
                anim = new Anim_Directional();
            } else {
                std::cout << "Unknown animation type: " << animType << std::endl;
                continue;
            }
            
            keystream >> *anim;
            anim->SetSpriteSheet(this);
            anim->SetName(name);
            anim->Reset();
            animations.emplace(name, anim);
            
            if (animCurrent) continue;
            animCurrent = anim;
            animCurrent->Play();
        }
    }
    file.close();
    return true;
};