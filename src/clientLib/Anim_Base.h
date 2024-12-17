#pragma once
#include <string> 
using Frame = unsigned int;

class SpriteSheet;
class Anim_Base {
    friend class SpriteSheet;
public:
    Anim_Base();
    virtual ~Anim_Base();

    void SetName(const std::string& _name);
    bool SetFrameCurrent(Frame _current);
    void SetFrameStart(Frame _start);
    void SetFrameEnd(Frame _end);
    void SetFrameRow(Frame _row);
    void SetActionStart(int _start);
    void SetActionEnd(int _end);
    void SetFrameTime(float _frame);
    void SetLooping(bool _loop);
    void SetSpriteSheet(SpriteSheet* _sheet);

    SpriteSheet* GetSpriteSheet();

    Frame GetFrame();
    Frame GetStartFrame();
    Frame GetEndFrame();
    unsigned int GetFrameRow();
    int GetActionStart();
    int GetActionEnd();
    float GetElapsedTime();
    float GetFrameTime();
    bool IsPlaying();
    bool IsLooping();
    bool IsInAction();
    bool CheckMoved();
    std::string GetName();

    void Play();
    void Pause();
    void Stop();
    void Reset();

    virtual void Update(float _dt);
    friend std::stringstream& operator >>(std::stringstream& _stream, Anim_Base& _anim) {
        _anim.ReadIn(_stream);
        return _stream;
    };
protected:    
    virtual void FrameStep() = 0;
    virtual void CropSprite() = 0;
    virtual void ReadIn(std::stringstream& _stream) = 0;
    Frame frameCurrent;
    Frame frameStart;
    Frame frameEnd;
    Frame frameRow;

    int frameActionStart;
    int frameActionEnd;
    float frameTime;
    float elapsedTime;

    bool playing;
    bool loop;
    bool hasMoved;

    std::string name;

    SpriteSheet* spriteSheet;
};