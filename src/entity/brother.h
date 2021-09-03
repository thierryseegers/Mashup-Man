#pragma once

#include "command.h"
#include "entity/character.h"
#include "entity/entity.h"
#include "layer.h"
#include "scene.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

namespace entity
{

class brother
    : public friendly<character>
{
public:
    enum class size
    {
        small,
        big
    };

    enum class attribute
    {
        plain,
        fiery
    };

    enum class motion
    {
        moving,
        still
    };

    enum class liveness
    {
        alive,
        dead
    };

    brother(
        sprite const& sprite_);

    virtual ~brother() = default;

    void set_direction(
        sf::Vector2f const& direction);

    void fire();

    void consume_mushroom();

    void consume_flower();

    void hit();

    sf::Vector2f heading;

protected:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    void update_sprite();

    void shoot_fireball(
        layer::projectiles& layer) const;

    sf::IntRect (*still_sprite_rect_)(
        brother::size const,
        brother::attribute const,
        brother::liveness const);

    std::vector<sf::IntRect> (*animated_sprite_rects_)(
        brother::size const,
        brother::attribute const);

    size size_;
    attribute attribute_;
    motion motion_;
    liveness liveness_;

    sf::Time fire_cooldown;
    sf::Time fire_countdown;
    bool firing;
};

class mario
    : public brother
{
public:
    mario();
};

class luigi
    : public brother
{
public:
    luigi();
};

}
