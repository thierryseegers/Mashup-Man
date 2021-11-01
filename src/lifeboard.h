#pragma once

#include "sprite.h"

#include <SFML/Graphics.hpp>

#include <vector>

class lifeboard
    : public sf::Drawable
    , public sf::Transformable
{
public:
    struct info
    {
        info(
            int const lives,
            sprite const& sprite_)
            : lives{lives}
            , sprite{sprite_}
        {}

        int lives;
        sprite sprite;
    };

    size_t grant(
        int const lives,
        sprite const& sprite);

    int take(
        size_t const i);

    virtual void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

private:
    mutable std::vector<info> infos;
};
