#pragma once

#include "command.h"
#include "resources.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

#include <vector>
class sprite_t :
    public sf::Transformable,
    public sf::Drawable
{
public:
    sprite_t(
        resources::texture const& texture,
        sf::IntRect const& texture_rect,
        float const scale);

    virtual ~sprite_t() = default;

    sf::FloatRect getGlobalBounds() const;

    virtual void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    virtual void update(
        sf::Time const&,
        commands_t&)
    {}

protected:
    sf::Sprite sprite;
};

class animated_sprite_t :
    public sprite_t
{
public:
    enum class repeat
    {
        back_and_forth, // TODO: implement.
        loop,
        none,
    };

    animated_sprite_t(
        resources::texture const& texture_sheet,
        std::vector<sf::IntRect> const& texture_rects,
        sf::Time const duration,
        repeat const repeat_,
        float const scale);

    virtual ~animated_sprite_t() = default;

    virtual void update(
        sf::Time const& dt,
        commands_t& commands) override;

protected:
    std::vector<sf::IntRect> const texture_rects;
    sf::Vector2i const frame_size;

    std::size_t const n_frames;
    std::size_t current_frame;

    sf::Time const duration;
    sf::Time elapsed;

    repeat const repeat_;
};
