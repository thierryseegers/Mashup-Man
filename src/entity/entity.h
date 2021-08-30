#pragma once

#include "command.h"
#include "scene.h"
#include "resources.h"
#include "utility.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

namespace entity
{

template<typename T>
class friendly : public T
{
public:
    using T::T;
};

template<typename T>
class hostile : public T
{
public:
    using T::T;
};

template<typename Sprite = scene::sprite_t>
class entity : public Sprite
{
public:
    explicit entity(
        resources::texture const& texture,
        sf::IntRect const& texture_rect,
        float const& scale = 1.f)
        : Sprite{texture, texture_rect}
    {
        utility::center_origin(Sprite::sprite);
        Sprite::sprite.setScale(scale, scale);
    }

    entity(
        resources::texture const& texture,
        sf::IntRect const& texture_rect,
        sf::Vector2i const frame_size,
        std::size_t const n_frames,
        sf::Time const duration,
        bool const repeat,
        float const& scale = 1.f)
        : Sprite(texture, texture_rect, frame_size, n_frames, duration, repeat)
    {
        utility::center_origin(Sprite::sprite);
        Sprite::sprite.setScale(scale, scale);
    }

    virtual ~entity() = default;

    virtual sf::FloatRect collision_bounds() const override
    {
        return Sprite::world_transform().transformRect(Sprite::sprite.getGlobalBounds());
    }

    void play_local_sound(
        commands_t& commands,
        resources::sound_effect const se) const
    {
        commands.push(make_command<scene::sound_t>([=](scene::sound_t& s, sf::Time const&)
        {
            s.play(se, Sprite::node::world_position());
        }));
    }

    sf::Vector2f velocity;

protected:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override
    {
        sf::Transformable::move(velocity * dt.asSeconds());

        Sprite::update_self(dt, commands);
    }
};

}
