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
    , public sf::Transformable
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
        sf::IntRect const& texture_rect);

    sprite(
        resources::texture const& texture_sheet,
        std::vector<sf::IntRect> const& texture_rects,
        sf::Time const duration,
        repeat const repeat_ = repeat::loop);

    ~sprite() = default;

    void still(
        sf::IntRect const& texture_rect);

    void animate(
        std::vector<sf::IntRect> const& texture_rects,
        sf::Time const duration,
        repeat const repeat_);

    void flip();

    void unflip();

    void set_color(
        sf::Color const color);

    sf::FloatRect global_bounds() const;

    void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    void update(
        sf::Time const&);

protected:
    sprite(
        resources::texture const& texture_sheet);

    sf::Sprite sprite_;

    // Because this obejct can be copied/moved, `updater` must not capture the `this` 
    // pointer but rather be given an instance of `sprite` to work with.
    std::function<void (sf::Time const&, sprite&)> updater;

    bool flipped;

    // These two pieces of information are used to keep an animation `flowing` when 
    // calling `animate` with a different texture_rect.
    size_t current_frame;
    sf::Time elapsed;
};
