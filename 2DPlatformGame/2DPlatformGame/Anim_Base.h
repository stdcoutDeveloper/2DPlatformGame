#pragma once

#include <string>

class SpriteSheet; // forward declaration.

typedef unsigned int Frame;

class Anim_Base
{
private:
    friend class SpriteSheet;

public:
    Anim_Base();
    virtual ~Anim_Base();

    void SetSpriteSheet(SpriteSheet* sheet);
    void SetFrame(Frame frame);
    void SetStartFrame(Frame frame);
    void SetEndFrame(Frame frame);
    void SetFrameRow(Frame row);
    void SetActionStart(Frame frame);
    void SetActionEnd(Frame frame);
    void SetFrameTime(float time);
    void SetLooping(bool loop);
    void SetName(const std::string& name);

    SpriteSheet* GetSpriteSheet() const;
    Frame GetFrame() const;
    Frame GetStartFrame() const;
    Frame GetEndFrame() const;
    Frame GetFrameRow() const;
    int GetActionStart() const;
    int GetActionEnd() const;
    float GetFrameTime() const;
    float GetElapsedTime() const;
    bool IsLooping() const;
    bool IsPlaying() const;
    bool IsInAction() const;
    std::string GetName() const;

    void Play();
    void Pause();
    void Stop();
    void Reset();

    virtual void Update(const float& deltaTime);

	/**
	 * \brief Overload operator >>
	 */
    friend std::stringstream& operator >>(
        std::stringstream& stream, Anim_Base& a)
    {
        a.ReadIn(stream);
        return stream;
    }

protected:
    virtual void FrameStep() = 0;
    virtual void CropSprite() = 0;
    virtual void ReadIn(std::stringstream& stream) = 0;

    Frame current_frame_;
    Frame start_frame_;
    Frame end_frame_;
    Frame frame_row_;
    int action_start_frame_;
    int action_end_frame_;
    float frame_time_;
    float elapsed_time_;
    bool is_loop_;
    bool is_playing_;

    std::string name_;

    SpriteSheet* sprite_sheet_;
};
