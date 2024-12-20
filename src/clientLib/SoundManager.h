#pragma once
#include <SFML/Audio.hpp>
#include "Utilities.h"
#include "SoundProps.h"
#include "AudioManager.h"
#include <unordered_map>
#include <functional>
#include <vector>

using SoundID = int;

enum class StateType;

struct SoundInfo {
    SoundInfo(const std::string& _name) : name(_name), manualPaused(false) {};
    std::string name;
    bool manualPaused;
};

using SoundProperties = std::unordered_map<std::string, SoundProps>;
using SoundContainer = std::unordered_map<SoundID, std::pair<SoundInfo, sf::Sound*>>;
using Sounds = std::unordered_map<StateType, SoundContainer>;
using RecycledSounds = std::vector<std::pair<std::pair<SoundID, std::string>, sf::Sound*>>;
using MusicContainer =  std::unordered_map<StateType, std::pair<SoundInfo, sf::Music*>>;

class SoundManager {
public:
    SoundManager(AudioManager* _audioMgr);
    ~SoundManager();

    void ChangeState(const StateType& _type);
    void RemoveState(const StateType& _type);

    void Update(float _dt);

    SoundID Play(const std::string& _sound, const sf::Vector3f& _pos, 
        bool loop = false, bool relative = false);

    bool Play(const SoundID& _sound);
    bool Pause(const SoundID& _sound);
    bool Stop(const SoundID& _sound);

    bool PlayMusic(const std::string& _musicID, float volume = 100.f, bool loop = false);
    bool PlayMusic(const StateType& _state);
    bool PauseMusic(const StateType& _state);
    bool StopMusic(const StateType& _state);

    bool SetPosition(const SoundID& _sound, const sf::Vector3f& _pos);
    bool IsPlaying(const SoundID& _sound);
    SoundProps* GetSoundProperties(const std::string& _sound);

    static const int MaxSounds = 150;
    static const int SoundCache = 75;       
private:
    bool LoadProperties(const std::string& _file);
    void PauseAll(const StateType& _state);
    void UnpauseAll(const StateType& _state);

    sf::Sound* CreateSound(SoundID& _id, const std::string& _file);
    void SetupSound(sf::Sound* _sound, SoundProps* _props, bool loop = false, bool relative = false);
    void RecycleSound(const SoundID& _id, sf::Sound* _sound, const std::string& _name);

    void Cleanup();

    Sounds audio;
    SoundProperties props;
    MusicContainer music;
    RecycledSounds recycled;
    StateType currentState;

    AudioManager* audioMgr;
    SoundID lastID;
    
    unsigned int numSounds;
    float elapsed;
};