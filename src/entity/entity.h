#pragma once

#include "command.h"
#include "scene.h"
#include "sprite.h"
#include "utility.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>

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

struct entity
    : public scene::node
{
public:
    virtual ~entity() = default;

    virtual sf::FloatRect collision_bounds() const override
    {
        return world_transform().transformRect(sprite->getGlobalBounds());
    }

    void play_local_sound(
        commands_t& commands,
        resources::sound_effect const se) const
    {
        commands.push(make_command<scene::sound_t>([=](scene::sound_t& s, sf::Time const&)
        {
            s.play(se, world_position());
        }));
    }

    sf::Vector2f velocity;

protected:
    std::unique_ptr<sprite_t> sprite;

    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override
    {
        sf::Transformable::move(velocity * dt.asSeconds());

        sprite->update(dt, commands);
    }

    virtual void draw_self(
        sf::RenderTarget& target,
        sf::RenderStates states) const override
    {
        sprite->draw(target, states);
    }

};

}
