#pragma once

#include "command.h"
#include "resources.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

#include <array>
#include <functional>
#include <vector>

class sprite
    : public sf::Drawable
{
public:
    enum class repeat
    {
        back_and_forth, // TODO: implement.
        loop,
        none,
    };

    sprite(
        resources::texture const& texture_sheet,
        sf::IntRect const& texture_rect,
        float const scale_factor = 1.f);

    sprite(
        resources::texture const& texture_sheet,
        std::vector<sf::IntRect> const& texture_rects,
        sf::Time const duration,
        repeat const repeat_ = repeat::loop,
        float const scale_factor = 1.f);

    ~sprite() = default;

    void still(
        sf::IntRect const& texture_rect);

    void animate(
        std::vector<sf::IntRect> const& texture_rects,
        sf::Time const duration,
        repeat const repeat_);

    void flip();

    void unflip();

    void set_rotation(
        float const angle);

    void set_color(
        sf::Color const color);

    sf::FloatRect global_bounds() const;

    void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    void update(
        sf::Time const&,
        commands_t&);

protected:
    sf::Sprite sprite_;

    // TODO: figure out why the workaround of this function getting passed 'this->sprite_' is required 
    // as opposed to to just referring to 'this->sprite_' within the lambda given.
    std::function<void (sf::Time const&, commands_t&, sf::Sprite&)> updater;

    float scale_factor;
    bool flipped;
};
