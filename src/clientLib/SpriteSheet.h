#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "TextureManager.h"
#include "Utilities.h"
#include "Anim_Directional.h"
#include <unordered_map>

using Animations = std::unordered_map<std::string, Anim_Base*>;

class SpriteSheet {
public:    
    SpriteSheet(TextureManager* _textMgr);
    ~SpriteSheet();

    void CropSprite(const sf::IntRect& _rect);
    const sf::Vector2u& GetSpriteSize() const;
    const sf::Vector2f& GetSpritePosition() const;
    void SetSpriteSize(const sf::Vector2u& _spriteSize);
    void SetSpritePosition(const sf::Vector2f& _spritePosition);

    void SetDirection(const Direction& _dir);
    Direction GetDirection() const;
    void SetSpriteSpacing(const sf::Vector2f& _spacing);
    void SetSheetPadding(const sf::Vector2f& _padding);
    const sf::Vector2f& GetSpriteSpacing() const;
    const sf::Vector2f& GetSheetPadding() const;

    bool LoadSheet(const std::string& _file);
    void ReleaseSheet();
    
    Anim_Base* GetCurrentAnimation();
    bool SetCurrentAnimation(const std::string& _name, bool _play = false, bool _loop = false);

    void Update(float _dt);
    void Draw(sf::RenderWindow* _wind);
private:
    std::string texture;
    sf::Sprite sprite;
    sf::Vector2u spriteSize;
    sf::Vector2f spriteScale;
    Direction direction;

    sf::Vector2f sheetPadding;
    sf::Vector2f spriteSpacing;

    std::string animType;
    Animations animations;
    Anim_Base* animCurrent;

    TextureManager* textMgr;
};