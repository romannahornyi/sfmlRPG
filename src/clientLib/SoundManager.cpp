#include "SoundManager.h"
#include "StateManager.h"

SoundManager::SoundManager(AudioManager* _audioMgr) : audioMgr(_audioMgr), lastID(0),  
    elapsed(0.f), numSounds(0) {};

SoundManager::~SoundManager() { Cleanup(); };

void SoundManager::ChangeState(const StateType& _state) {
    PauseAll(currentState);
    UnpauseAll(_state);
    currentState = _state;

    if (music.find(currentState) != music.end()) return;
    sf::Music* m = new sf::Music();
    SoundInfo info("");
    music.emplace(currentState, std::make_pair(info, m)); 
};

void SoundManager::RemoveState(const StateType& _state) {
    auto stateSounds = audio.find(_state)->second;
    for (auto& s : stateSounds) {
        RecycleSound(s.first, s.second.second, s.second.first.name);
    }
    audio.erase(_state);
    auto m = music.find(_state);
    if (m == music.end()) return;
    if (m->second.second) {
        delete m->second.second; 
        --numSounds;
    }
    music.erase(_state);
};

void SoundManager::Cleanup() {
    for (auto& state : audio) {
        for (auto& sound : state.second) {
            audioMgr->ReleaseResource(sound.second.first.name);
            delete sound.second.second;
        }
    }
    audio.clear();
    for (auto& itr : recycled) {
        audioMgr->ReleaseResource(itr.first.second);
        delete itr.second;
    }
    recycled.clear();
    for (auto& itr : music) {
        if (itr.second.second) {
            delete itr.second.second;
        }
    }
    music.clear();

    props.clear();
    numSounds = 0;
    lastID = 0;
};

void SoundManager::Update(float _dt) {
    elapsed += _dt;
    if (elapsed < 0.33f) return;
    auto& container = audio[currentState];
    for (auto itr = container.begin(); itr != container.end(); ) {
        if (!itr->second.second->getStatus()) {
            RecycleSound(itr->first, itr->second.second, itr->second.first.name);
            itr = container.erase(itr);
            continue;
        }
        ++itr;
    }
    auto m = music.find(currentState);
    if (m == music.end()) return;
    if (!m->second.second) return;
    if (m->second.second->getStatus()) return;
    delete m->second.second;
    m->second.second = nullptr;
    --numSounds;
};

SoundID SoundManager::Play(const std::string& _sound, const sf::Vector3f& _pos, bool _loop, bool _relative) {
    SoundProps* props = GetSoundProperties(_sound);
    if (!props) return -1;
    SoundID id;
    sf::Sound* s = CreateSound(id, props->audioName);
    if (!s) return -1;
    SetupSound(s, props, _loop, _relative);
    s->setPosition(_pos);
    SoundInfo info(props->audioName);
    audio[currentState].emplace(id, std::make_pair(info, s));
    s->play();
    return id;
};

bool SoundManager::Play(const SoundID& _sound) {
    auto itr = audio[currentState].find(_sound);
    if (itr == audio[currentState].end()) return false;
    itr->second.second->play();
    itr->second.first.manualPaused = false;
    return true;
};

bool SoundManager::Pause(const SoundID& _sound) {
    auto itr = audio[currentState].find(_sound);
    if (itr == audio[currentState].end()) return false;
    itr->second.second->pause();
    itr->second.first.manualPaused = true;
    return true;
};

bool SoundManager::Stop(const SoundID& _sound) {
    auto itr = audio[currentState].find(_sound);
    if (itr == audio[currentState].end()) return false;
    itr->second.second->stop();
    itr->second.first.manualPaused = true;
    return true;
};

bool SoundManager::PlayMusic(const std::string& _id, float _volume, bool _loop) {
    auto itr = music.find(currentState);
    if (itr == music.end()) return false;
    std::string path = audioMgr->GetPath(_id);
    if (path == "");
    if (!itr->second.second) {
        itr->second.second = new sf::Music();
        ++numSounds;
    }  
    sf::Music* m = itr->second.second;
    if (!m->openFromFile(Utils::GetWorkingDirectory() + path)) {
        delete m;
        --numSounds;
        itr->second.second = nullptr;
        std::cerr << "[SoundManager] Failed opening music from file: " << path << std::endl;
        return false;
    }
    m->setLoop(false);
    m->setVolume(_volume);
    m->setRelativeToListener(true);
    m->play();
    itr->second.first.name = _id;
    return true;
};

bool SoundManager::PlayMusic(const StateType& _state) {
    auto itr = music.find(_state);
    if (itr == music.end()) return false;
    if (!itr->second.second) return false;
    itr->second.second->play();
    itr->second.first.manualPaused = false;
    return true;
};

bool SoundManager::PauseMusic(const StateType& _state) {
    auto itr = music.find(_state);
    if (itr == music.end()) return false;
    if (!itr->second.second) return false;
    itr->second.second->pause();
    itr->second.first.manualPaused = true;
    return true;
};

bool SoundManager::StopMusic(const StateType& _state) {
    auto itr = music.find(_state);
    if (itr == music.end()) return false;
    if (!itr->second.second) return false;
    itr->second.second->stop();
    delete itr->second.second;
    itr->second.second = nullptr;
    --numSounds;
    return true;
};

bool SoundManager::SetPosition(const SoundID& _sound, const sf::Vector3f& _pos) {
    auto& container = audio[currentState];
    auto itr = container.find(_sound);
    if (itr == container.end()) return false;
    itr->second.second->setPosition(_pos);
    return true;
};

bool SoundManager::IsPlaying(const SoundID& _sound) {
    auto& container = audio[currentState];
    auto itr = container.find(_sound);
    return (itr != container.end() ? itr->second.second->getStatus() : false);
};

SoundProps* SoundManager::GetSoundProperties(const std::string& _file) {
    auto sp = props.find(_file);
    if (sp == props.end()) {
        if (!LoadProperties(_file)) return nullptr;
        sp = props.find(_file);
    }
    return &sp->second;
};

bool SoundManager::LoadProperties(const std::string& _sound) {
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + "media/Sounds/" + _sound + ".sound");
    if (!file.is_open()) {
        std::cout << "[SoundManager] failed to load sound from file: " << _sound << std::endl;
        return false;
    }
    SoundProps prps("");
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "Audio") {
            keystream >> prps.audioName;
        } else if (type == "Volume") {
            keystream >> prps.volume;
        } else if (type == "Pitch") {
            keystream >> prps.pitch;
        } else if (type == "Distance") {
            keystream >> prps.minDistance;
        } else if (type == "Attenuation") {
            keystream >> prps.attenuation;
        } else {
            //
        }
    }
    file.close();
    if (prps.audioName == "") return false;
    props.emplace(_sound, prps);
    return true;
};

void SoundManager::PauseAll(const StateType& _state) {
    auto& container = audio[_state];
    for (auto itr = container.begin(); itr != container.end(); ) {
        if (!itr->second.second->getStatus()) {
            RecycleSound(itr->first, itr->second.second, itr->second.first.name);
            itr = container.erase(itr);
            continue;
        }
        itr->second.second->pause();
        ++itr;
    }
    auto msc = music.find(_state);
    if (msc == music.end()) return;
    if (!msc->second.second) return;
    msc->second.second->pause();
};

void SoundManager::UnpauseAll(const StateType& _state) {
    auto& container = audio[_state];
    for (auto& itr : container) {
        if (itr.second.first.manualPaused) continue;
        itr.second.second->play();
    }
    auto msc = music.find(_state);
    if (msc == music.end() || !msc->second.second) return;
    msc->second.second->play();
};

sf::Sound* SoundManager::CreateSound(SoundID& _id, const std::string& _audio) {
    sf::Sound* sound = new sf::Sound();
    if (!recycled.empty() && (numSounds >= MaxSounds || (int)recycled.size() >= SoundCache)) {
        auto itr = recycled.begin();
        while (itr != recycled.end()) {
            if (itr->first.second == _audio) break;
            ++itr;
        }
        if (itr == recycled.end()) {
            auto elem = recycled.begin();
            auto eid = elem->first.first;
            audioMgr->ReleaseResource(itr->first.second);
            audioMgr->RequireResource(_audio);
            sound = elem->second;
            sound->setBuffer(*audioMgr->GetResource(_audio));
            recycled.erase(elem);
        } else {
            _id = itr->first.first;
            sound = itr->second;
            recycled.erase(itr);
        }
        return sound;
    }

    if (numSounds < MaxSounds) {
        if (audioMgr->RequireResource(_audio)) {
            sound = new sf::Sound();
            _id = lastID;
            ++lastID;
            ++numSounds;
            sound->setBuffer(*audioMgr->GetResource(_audio));
            return sound;
        }
    }
    std::cerr << "[SoundManager] Failed to create sound from file: " << _audio << std::endl;
    return nullptr;
};

void SoundManager::SetupSound(sf::Sound* _sound, SoundProps* _props, bool _loop, bool _relative) {
    _sound->setVolume(_props->volume);
    _sound->setAttenuation(_props->attenuation);
    _sound->setLoop(_loop);
    _sound->setRelativeToListener(_relative);
    _sound->setPitch(_props->pitch);
    _sound->setMinDistance(_props->minDistance);
};

void SoundManager::RecycleSound(const SoundID& _id, sf::Sound* _sound, const std::string& _audio) {
    recycled.emplace_back(std::make_pair(std::make_pair(_id, _audio), _sound));
};