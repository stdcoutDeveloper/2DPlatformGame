#include "Anim_Base.h"
#include "SpriteSheet.h"

Anim_Base::Anim_Base() : current_frame_(0), start_frame_(0), end_frame_(0),
                         frame_row_(0), action_start_frame_(-1), action_end_frame_(-1), frame_time_(0.0f),
                         elapsed_time_(0.0f), is_loop_(false), is_playing_(false), sprite_sheet_(nullptr)
{
}

Anim_Base::~Anim_Base() = default;

void Anim_Base::SetFrame(Frame frame)
{
    if ((frame >= start_frame_ && frame <= end_frame_) ||
        (frame >= end_frame_ && frame <= start_frame_))
        current_frame_ = frame;
}

bool Anim_Base::IsInAction() const
{
    if (action_start_frame_ == -1 || action_end_frame_ == -1)
        return true;

    return (current_frame_ >= static_cast<unsigned int>(action_start_frame_) && current_frame_ <= static_cast<unsigned
        int>(action_end_frame_));
}

void Anim_Base::Reset()
{
    current_frame_ = start_frame_;
    elapsed_time_ = 0.0f;
    CropSprite();
}

void Anim_Base::Update(const float& deltaTime)
{
    if (!is_playing_)
        return;

    elapsed_time_ += deltaTime;
    if (elapsed_time_ < frame_time_)
        return;

    FrameStep();
    CropSprite();
    elapsed_time_ = 0;
}

void Anim_Base::SetSpriteSheet(SpriteSheet* sheet)
{
    sprite_sheet_ = sheet;
}

void Anim_Base::SetStartFrame(Frame frame)
{
    start_frame_ = frame;
}

void Anim_Base::SetEndFrame(Frame frame)
{
    end_frame_ = frame;
}

void Anim_Base::SetFrameRow(Frame row)
{
    frame_row_ = row;
}

void Anim_Base::SetActionStart(Frame frame)
{
    action_start_frame_ = frame;
}

void Anim_Base::SetActionEnd(Frame frame)
{
    action_end_frame_ = frame;
}

void Anim_Base::SetFrameTime(float time)
{
    frame_time_ = time;
}

void Anim_Base::SetLooping(bool loop)
{
    is_loop_ = loop;
}

void Anim_Base::SetName(const std::string& name)
{
    name_ = name;
}

SpriteSheet* Anim_Base::GetSpriteSheet() const
{
    return sprite_sheet_;
}

Frame Anim_Base::GetFrame() const
{
    return current_frame_;
}

Frame Anim_Base::GetStartFrame() const
{
    return start_frame_;
}

Frame Anim_Base::GetEndFrame() const
{
    return end_frame_;
}

Frame Anim_Base::GetFrameRow() const
{
    return frame_row_;
}

int Anim_Base::GetActionStart() const
{
    return action_start_frame_;
}

int Anim_Base::GetActionEnd() const
{
    return action_end_frame_;
}

float Anim_Base::GetFrameTime() const
{
    return frame_time_;
}

float Anim_Base::GetElapsedTime() const
{
    return elapsed_time_;
}

std::string Anim_Base::GetName() const
{
    return name_;
}

bool Anim_Base::IsLooping() const
{
    return is_loop_;
}

bool Anim_Base::IsPlaying() const
{
    return is_playing_;
}

void Anim_Base::Play()
{
    is_playing_ = true;
}

void Anim_Base::Pause()
{
    is_playing_ = false;
}

void Anim_Base::Stop()
{
    is_playing_ = false;
    Reset();
}
