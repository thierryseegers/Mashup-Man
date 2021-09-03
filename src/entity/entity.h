#pragma once

#include "command.h"
#include "direction.h"
#include "scene.h"
#include "sprite.h"
#include "utility.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>

namespace entity
{

template<typename T>
class friendly
    : public T
{
public:
    using T::T;
};

template<typename T>
class hostile
    : public T
{
public:
    using T::T;
};

class entity
    : public scene::node
{
public:
    entity(
        sprite sprite_,
        float const speed = 0,
        direction const heading_ = direction::still);

    virtual ~entity() = default;

    virtual sf::FloatRect collision_bounds() const override;

    direction heading() const;

    void head(
        direction const d);

    void play_local_sound(
        commands_t& commands,
        resources::sound_effect const se) const;

protected:
    sprite sprite_;

    direction heading_;
    float speed;

    virtual void update_sprite();

    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    virtual void draw_self(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;
};

}
