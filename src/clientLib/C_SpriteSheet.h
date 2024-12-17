#pragma once
#include "C_Drawable.h"
#include "SpriteSheet.h"

class C_SpriteSheet : public C_Drawable {
public:
    C_SpriteSheet() : C_Drawable(Component::SpriteSheet), spriteSheet(nullptr) {};
    ~C_SpriteSheet() {
        if (spriteSheet) {
            delete spriteSheet;
        }
    };

    void Create(TextureManager* _textMgr, const std::string& _name = "") {
        if (spriteSheet) return;
        spriteSheet = new SpriteSheet(_textMgr);
        spriteSheet->LoadSheet("media/Spritesheets/" + (_name != "" ? _name : sheetName) + ".sheet");
    };

    SpriteSheet* GetSpriteSheet() { return spriteSheet; };

    void UpdatePosition(const sf::Vector2f& _vec) {
        spriteSheet->SetSpritePosition(_vec);
    };

    const sf::Vector2u& GetSize() {
        return spriteSheet->GetSpriteSize();
    };

    void Draw(sf::RenderWindow* _wind) {
        if (!spriteSheet) return;
        spriteSheet->Draw(_wind);
    };
private:
    SpriteSheet* spriteSheet;
    std::string sheetName;
};