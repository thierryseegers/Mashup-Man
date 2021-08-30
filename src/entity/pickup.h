#pragma once

#include "entity/entity.h"
#include "entity/brother.h"
#include "resources.h"

#include <SFML/Graphics/Rect.hpp>

namespace entity::pickup
{

template<typename Sprite = scene::sprite_t>
class pickup : public entity<Sprite>
{
public:
    pickup(
        resources::texture const& texture,
        sf::IntRect const& texture_rect,
        float const& scale = 1.f)
        : entity<Sprite>{texture, texture_rect, scale}
    {}

    pickup(
        resources::texture const& texture,
        sf::IntRect const& texture_rect,
        sf::Vector2i const frame_size,
        std::size_t const n_frames,
        sf::Time const duration,
        bool const repeat,
        float const& scale = 1.f)
        : entity<Sprite>{texture, texture_rect, frame_size, n_frames, duration, repeat, scale}
    {}

    virtual void apply(
        brother& b) const = 0;
};

class coin : public pickup<scene::animated_sprite_t>
{
public:
    coin();

    virtual void apply(
        brother& b) const override;
};

class mushroom : public pickup<>
{
public:
    mushroom();

    virtual void apply(
        brother& b) const override;
};

class flower : public pickup<scene::animated_sprite_t>
{
public:
    flower();

    virtual void apply(
        brother& b) const override;
};

// class increase_spread : public pickup
// {
// public:
//     increase_spread();

//     virtual void apply(
//         brother& leader) const override;
// };

// class increase_fire_rate : public pickup
// {
// public:
//     increase_fire_rate();

//     virtual void apply(
//         brother& leader) const override;
// };

}
