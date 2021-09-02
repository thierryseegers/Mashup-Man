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
    entity(
        sprite sprite_);

    virtual ~entity() = default;

    virtual sf::FloatRect collision_bounds() const override;

    void play_local_sound(
        commands_t& commands,
        resources::sound_effect const se) const;

    sf::Vector2f velocity;

protected:
    sprite sprite_;

    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    virtual void draw_self(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;
};

}
