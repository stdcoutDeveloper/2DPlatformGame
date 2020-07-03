#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

class DebugOverlay
{
public:
    DebugOverlay()
    {
        is_debug_ = false;
    }

    void Add(sf::Drawable* drawable)
    {
        drawables_.push_back(drawable);
    }

    void Draw(sf::RenderWindow* wind)
    {
        while (drawables_.begin() != drawables_.end())
        {
            wind->draw(*drawables_.back());
            delete drawables_.back();
            drawables_.pop_back();
        }
    }

    bool Debug() const
    {
        return is_debug_;
    }

    void SetDebug(const bool& val)
    {
        is_debug_ = val;
    }

private:
    std::vector<sf::Drawable*> drawables_;
    bool is_debug_;
};
