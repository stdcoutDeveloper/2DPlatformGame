#include "Anim_Directional.h"
#include "SpriteSheet.h"

void Anim_Directional::CropSprite()
{
    const sf::IntRect rect(sprite_sheet_->GetSpriteSize().x * current_frame_,
                           sprite_sheet_->GetSpriteSize().y * (frame_row_ + static_cast<short>(sprite_sheet_->
                                   GetDirection())
                           ),
                           sprite_sheet_->GetSpriteSize().x, sprite_sheet_->GetSpriteSize().y);
    sprite_sheet_->CropSprite(rect);
}

void Anim_Directional::FrameStep()
{
    if (start_frame_ < end_frame_) { ++current_frame_; }
    else { --current_frame_; }

    if ((start_frame_ < end_frame_ && current_frame_ > end_frame_) ||
        (start_frame_ > end_frame_ && current_frame_ < end_frame_))
    {
        if (is_loop_)
        {
            current_frame_ = start_frame_;
            return;
        }
        current_frame_ = end_frame_;
        Pause();
    }
}

void Anim_Directional::ReadIn(std::stringstream& l_stream)
{
    l_stream >> start_frame_ >> end_frame_ >> frame_row_
        >> frame_time_ >> action_start_frame_ >> action_end_frame_;
}
