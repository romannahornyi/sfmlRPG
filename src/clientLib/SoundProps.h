#pragma once

struct SoundProps {
    SoundProps(std::string _name) : audioName(_name), volume(100.f), pitch(1.f),
        minDistance(10.f), attenuation(10.f) {};

    std::string audioName;
    float volume;
    float pitch;
    float minDistance;
    float attenuation;
};