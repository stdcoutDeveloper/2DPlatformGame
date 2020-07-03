#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "TextureManager.h"
#include "Anim_Directional.h"
#include "Utilities.h"
#include <unordered_map>

using Animations = std::unordered_map<std::string, Anim_Base*>;

class SpriteSheet
{
public:
    SpriteSheet(TextureManager* textMgr);
    ~SpriteSheet();

    void CropSprite(const sf::IntRect& rect);
    sf::Vector2i GetSpriteSize() const;
    sf::Vector2f GetSpritePosition() const;
    void SetSpriteSize(const sf::Vector2i& size);
    void SetSpritePosition(const sf::Vector2f& pos);

    void SetDirection(const Direction& dir);
    Direction GetDirection() const;

    bool LoadSheet(const std::string& file);
    void ReleaseSheet();

    Anim_Base* GetCurrentAnim() const;
    bool SetAnimation(const std::string& name,
                      const bool& isPlay = false,
                      const bool& isLoop = false);

    void Update(const float& deltaTime) const;
    void Draw(sf::RenderWindow* wnd) const;
private:
    std::string texture_;
    sf::Sprite sprite_;
    sf::Vector2i sprite_size_;
    sf::Vector2f sprite_scale_;
    Direction direction_;

    std::string anim_type_;
    Animations animations_;
    Anim_Base* animation_current_;

    TextureManager* texture_mgr_;
};
