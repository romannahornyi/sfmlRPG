#pragma once
#include <SFML/Graphics/Font.hpp>
#include "ResourceManager.h"

class FontManager : public ResourceManager<FontManager, sf::Font> {
public:
    FontManager() : ResourceManager("fonts.cfg") {};

    sf::Font* Load(const std::string& _file) {
        sf::Font* font = new sf::Font();
        if (!font->loadFromFile(Utils::GetWorkingDirectory() + _file)) {
            delete font;
            font = nullptr;
            std::cerr << "[FontManager] failed to load font from: " << _file << std::endl;
        }
        return font; 
    };
private:

};