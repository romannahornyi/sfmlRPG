#pragma once
#include "ResourceManager.h"
#include <SFML/Audio/SoundBuffer.hpp>

class AudioManager : public ResourceManager<AudioManager, sf::SoundBuffer> {
public:
    AudioManager() : ResourceManager("audio.cfg") {};

    sf::SoundBuffer* Load(const std::string& _file) {
        sf::SoundBuffer* s = new sf::SoundBuffer();
        if (!s->loadFromFile(Utils::GetWorkingDirectory() + _file)) {
            delete s;
            s = nullptr;
            std::cerr << "Failed to load from file: " << _file << std::endl;
        }
        return s;
    };
};