#pragma once

#include "command.h"
#include "entity/entity.h"
#include "resources.h"
#include "scene.h"

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

namespace entity
{

class animation : public entity<scene::animated_sprite_t>
{
public:
    animation(
        resources::texture const& texture,
        sf::Vector2i const frame_size,
        std::size_t const n_frames,
        sf::Time const duration,
        bool const repeat,
        sf::Vector2f const position)
        : entity<scene::animated_sprite_t>{texture, frame_size, n_frames, duration, repeat}
    {
        setPosition(position - sf::Vector2f{frame_size.x / 2.f, frame_size.y / 2.f});
    }

    virtual ~animation() = default;

protected:
    void update_self(
        sf::Time const& dt,
        commands_t& commands)
    {
        if(current_frame >= n_frames)
        {
            remove = true;
        }
        else
        {
            entity<scene::animated_sprite_t>::update_self(dt, commands);
        }
    }
};

}
