#include "Anim_Directional.h"
#include "SpriteSheet.h"

void Anim_Directional::CropSprite() {
    sf::Vector2f padding = spriteSheet->GetSheetPadding();
    sf::Vector2f spacing = spriteSheet->GetSpriteSpacing();
    sf::IntRect rect(
        (spriteSheet->GetSpriteSize().x * frameCurrent) + padding.x + (spacing.x * frameCurrent),
        (spriteSheet->GetSpriteSize().y * (frameRow + (short)spriteSheet->GetDirection())) + 
        padding.y + ((frameRow + (short)spriteSheet->GetDirection()) * spacing.y),
        spriteSheet->GetSpriteSize().x, spriteSheet->GetSpriteSize().y
    );
    spriteSheet->CropSprite(rect);
};

void Anim_Directional::FrameStep() {
    bool b = SetFrameCurrent(frameCurrent + (frameStart <= frameEnd ? 1 : -1));
    if (b) return;
    if (loop) {
        SetFrameCurrent(frameStart);
    } else {
        SetFrameCurrent(frameEnd);
        Pause();
    }
};

void Anim_Directional::ReadIn(std::stringstream& _stream) {
    _stream >> frameStart >> frameEnd >> frameRow >> frameTime >>
        frameActionStart >> frameActionEnd;
};