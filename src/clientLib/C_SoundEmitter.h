#pragma once
#include "C_Base.h"
#include "SoundManager.h"
#include <array>

enum class EntitySound { None = -1, Footstep, Attack, Hurt, Death};

struct SoundParameters {
    static const int MaxSoundFrames = 5;
    SoundParameters() {
        for (int i = 0; i < MaxSoundFrames; ++i) {
            frames[i] = -1;
        }
    };
    std::string sound;
    std::array<int, MaxSoundFrames> frames;
};

class C_SoundEmitter : public C_Base {
public:
    static const int MaxEntitySounds = 4;

    C_SoundEmitter() : C_Base(Component::SoundEmitter), soundID(-1) {};
    void ReadIn(std::stringstream& _stream) {
        std::string mainDelimeter = ":";
        std::string frameDelimeter = ",";
        for (int i = 0; i < MaxEntitySounds; ++i) {
            std::string chunk;
            _stream >> chunk;
            if (chunk == "") break;

            std::string sound = chunk.substr(0, chunk.find(mainDelimeter));
            std::string frames = chunk.substr(chunk.find(mainDelimeter) + mainDelimeter.length());
            params[i].sound = sound;
            size_t pos = 0;
            unsigned int frameNum = 0;
            while (frameNum < SoundParameters::MaxSoundFrames) {
                pos = frames.find(frameDelimeter);
                int frame = -1;
                if (pos != std::string::npos) {
                    frame = stoi(frames.substr(0, pos));
                    frames.erase(0, pos + frameDelimeter.length());
                } else {
                    frame = stoi(frames);
                    params[i].frames[frameNum] = frame;
                    break;
                }
                params[i].frames[frameNum] = frame;
                ++frameNum;
            }
        }
    };

    SoundID GetSoundID() { return soundID; };
    void SetSoundID(const SoundID& _sound) { soundID = _sound; };

    const std::string& GetSound(const EntitySound& _sound) {
        static std::string empty = "";
        return ((int)_sound < MaxEntitySounds ? params[(int)_sound].sound : empty);
    };

    bool IsSoundFrame(const EntitySound& _snd, int _frame) {
        if ((int)_snd < 0 || (int)_snd >= MaxEntitySounds) return false;
        for (int i = 0; i < MaxEntitySounds; ++i) {
            if (params[(int)_snd].frames[i] == -1) return false;
            if (params[(int)_snd].frames[i] == _frame) return true;
        }
        return false;
    };
    SoundParameters* GetParameters() { return &params[0]; };
private:
    SoundID soundID;
    std::array<SoundParameters, MaxEntitySounds> params;
};