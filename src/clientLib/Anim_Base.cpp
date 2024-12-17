#include "Anim_Base.h"
#include "SpriteSheet.h"

Anim_Base::Anim_Base() : frameCurrent(0), frameStart(0), frameEnd(0), frameRow(0),
    frameActionStart(-1), frameActionEnd(-1), elapsedTime(0.f), frameTime(0.f), loop(false),
    playing(false), hasMoved(false) {};
    
Anim_Base::~Anim_Base() {};

void Anim_Base::SetSpriteSheet(SpriteSheet* _spriteSheet) {
    spriteSheet = _spriteSheet;
};

bool Anim_Base::SetFrameCurrent(unsigned int _frame) {
    if ((_frame >= frameStart && _frame >= frameEnd) ||
        (_frame <= frameStart && _frame <= frameEnd)) {
            frameCurrent = _frame;
            hasMoved = true;
            return true;
    }
    return false;
};

bool Anim_Base::IsInAction() {
    if (frameActionStart == -1 || frameActionEnd == -1) {
        return false;
    }
    return (frameCurrent >= frameActionStart && frameCurrent <= frameActionEnd);
};

void Anim_Base::Reset() {
    frameCurrent = frameStart;
    elapsedTime = 0.f;
    CropSprite();
};

void Anim_Base::Update(float _dt) {
    if (!playing) return;
    elapsedTime += _dt;
    if (elapsedTime < frameTime) return;
    FrameStep();
    CropSprite();
    elapsedTime = 0;
};

bool Anim_Base::CheckMoved() {
    bool result = hasMoved;
    hasMoved = false;
    return result;
};

void Anim_Base::Play() { playing = true; };
void Anim_Base::Pause() { playing = false; };
void Anim_Base::Stop() {
    playing = false;
    Reset();
};

void Anim_Base::SetName(const std::string& _name) { name = _name; };
void Anim_Base::SetActionStart(int _start) { frameActionStart = _start; };
void Anim_Base::SetActionEnd(int _end) { frameActionEnd = _end; };
void Anim_Base::SetFrameRow(Frame _row) { frameRow = _row; };
void Anim_Base::SetFrameStart(Frame _start) { frameStart = _start; };
void Anim_Base::SetFrameEnd(Frame _end) { frameEnd = _end; };
void Anim_Base::SetFrameTime(float _time) { frameTime = _time; };
void Anim_Base::SetLooping(bool _loop) { loop = _loop; };

Frame Anim_Base::GetFrame() { return frameCurrent; };
Frame Anim_Base::GetStartFrame() { return frameStart; };
Frame Anim_Base::GetEndFrame() { return frameEnd; };
unsigned int Anim_Base::GetFrameRow() { return frameRow; };
int Anim_Base::GetActionStart() { return frameActionStart; };
int Anim_Base::GetActionEnd() { return frameActionEnd; };
float Anim_Base::GetElapsedTime() { return elapsedTime; };
float Anim_Base::GetFrameTime() { return frameTime; };
bool Anim_Base::IsPlaying() { return playing; };
bool Anim_Base::IsLooping() { return loop; };
std::string Anim_Base::GetName() { return name; };