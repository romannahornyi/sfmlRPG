#pragma once
#include <SFML/Graphics/Texture.hpp>
#include "ResourceManager.h"

class TextureManager : public ResourceManager<TextureManager, sf::Texture> {
public:
    TextureManager() : ResourceManager("textures.cfg") {};
    sf::Texture* Load(const std::string& _path) {
        sf::Texture* texture = new sf::Texture();
        if (!texture->loadFromFile(Utils::GetWorkingDirectory() + _path)) {
            std::cout << "Failed loading texture from: " << _path << std::endl;
            delete texture;
            texture = nullptr;
        }
        return texture;
    };
};