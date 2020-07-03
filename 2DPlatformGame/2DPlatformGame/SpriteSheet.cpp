#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(TextureManager* textMgr)
    : sprite_scale_(1.f, 1.f), direction_(Direction::Right),
      animation_current_(nullptr), texture_mgr_(textMgr)
{
}

SpriteSheet::~SpriteSheet()
{
    ReleaseSheet();
}

void SpriteSheet::ReleaseSheet()
{
    texture_mgr_->ReleaseResource(texture_);
    animation_current_ = nullptr;
    while (animations_.begin() != animations_.end())
    {
        delete animations_.begin()->second;
        animations_.erase(animations_.begin());
    }
}

sf::Vector2i SpriteSheet::GetSpriteSize() const
{
    return sprite_size_;
}

sf::Vector2f SpriteSheet::GetSpritePosition() const
{
    return sprite_.getPosition();
}

Direction SpriteSheet::GetDirection() const
{
    return direction_;
}

Anim_Base* SpriteSheet::GetCurrentAnim() const
{
    return animation_current_;
}

void SpriteSheet::SetSpriteSize(const sf::Vector2i& size)
{
    sprite_size_ = size;
    sprite_.setOrigin(sprite_size_.x / 2.0f, sprite_size_.y * 1.0f);
}

void SpriteSheet::SetSpritePosition(const sf::Vector2f& pos) { sprite_.setPosition(pos); }

void SpriteSheet::SetDirection(const Direction& dir)
{
    if (dir == direction_) { return; }
    direction_ = dir;
    animation_current_->CropSprite();
}

void SpriteSheet::CropSprite(const sf::IntRect& rect)
{
    sprite_.setTextureRect(rect);
}

bool SpriteSheet::LoadSheet(const std::string& file)
{
    std::ifstream sheet;
    sheet.open(Utils::GetWorkingDirectory() + file);
    if (sheet.is_open())
    {
        ReleaseSheet(); // Release current sheet resources.
        std::string line;
        while (std::getline(sheet, line))
        {
            if (line[0] == '|') { continue; }
            std::stringstream keystream(line);
            std::string type;
            keystream >> type;
            if (type == "Texture")
            {
                if (!texture_.empty())
                {
                    std::cerr << "! Duplicate texture entries in: " << file << std::endl;
                    continue;
                }
                std::string texture;
                keystream >> texture;
                if (!texture_mgr_->RequireResource(texture))
                {
                    std::cerr << "! Could not set up the texture: " << texture << std::endl;
                    continue;
                }
                texture_ = texture;
                sprite_.setTexture(*texture_mgr_->GetResource(texture_));
            }
            else if (type == "Size")
            {
                keystream >> sprite_size_.x >> sprite_size_.y;
                SetSpriteSize(sprite_size_);
            }
            else if (type == "Scale")
            {
                keystream >> sprite_scale_.x >> sprite_scale_.y;
                sprite_.setScale(sprite_scale_);
            }
            else if (type == "AnimationType")
            {
                keystream >> anim_type_;
            }
            else if (type == "Animation")
            {
                std::string name;
                keystream >> name;
                if (animations_.find(name) != animations_.end())
                {
                    std::cerr << "! Duplicate animation(" << name << ") in: " << file << std::endl;
                    continue;
                }
                Anim_Base* anim = nullptr;
                if (anim_type_ == "Directional")
                {
                    anim = new Anim_Directional();
                }
                else
                {
                    std::cerr << "! Unknown animation type: " << anim_type_ << std::endl;
                    continue;
                }

                keystream >> *anim;
                anim->SetSpriteSheet(this);
                anim->SetName(name);
                anim->Reset();
                animations_.emplace(name, anim);

                if (animation_current_) { continue; }
                animation_current_ = anim;
                animation_current_->Play();
            }
        }
        sheet.close();
        return true;
    }
    std::cerr << "! Failed loading spritesheet: " << file << std::endl;
    return false;
}

bool SpriteSheet::SetAnimation(const std::string& name,
                               const bool& isPlay, const bool& isLoop)
{
    const auto itr = animations_.find(name);
    if (itr == animations_.end()) { return false; }
    if (itr->second == animation_current_) { return false; }
    if (animation_current_) { animation_current_->Stop(); }
    animation_current_ = itr->second;
    animation_current_->SetLooping(isLoop);
    if (isPlay) { animation_current_->Play(); }
    animation_current_->CropSprite();
    return true;
}

void SpriteSheet::Update(const float& deltaTime) const
{
    animation_current_->Update(deltaTime);
}

void SpriteSheet::Draw(sf::RenderWindow* wnd) const
{
    wnd->draw(sprite_);
}
