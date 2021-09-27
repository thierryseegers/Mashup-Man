#pragma once

#include "command.h"
#include "direction.h"
#include "entity/character.h"
#include "entity/power_up.h"
#include "layer.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

#include <memory>
#include <type_traits>

#if defined(WIN32)
    #undef small
#endif

namespace entity
{

class hero
    : public friendly<character>
{
public:
    hero(
        sprite sprite_,
        int const max_speed = 0);

    virtual ~hero() = default;

    // The hero may be impervious to damage when recently spawned.
    [[nodiscard]] bool immune() const override;

    // The direction towards which the hero is steered by the player.
    [[nodiscard]] direction steering() const;

    // The sprite to be used to represent this hero (e.g. on the lifeboard).
    virtual sf::Sprite default_sprite() = 0;

    // The hero has been hit, be it by a ghost or a projectile.
    virtual void hit() override;

    // The hero's attack (which may depend on a power-up).
    virtual void attack();

    // Pick up a power-up.
    virtual void pick_up(
        power_up::power_up*) = 0;

    // Steer the hero in a certain direction.
    void steer(
        direction const d);

protected:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    // The entity that this hero "transforms" into wen dead.
    virtual entity* dead() const = 0;

    direction steering_;

    sf::Time immunity;  // Immunity timer.
};

template<typename T>
class hero_1
    : public T
{
public:
    static_assert(
        std::is_base_of<hero, T>::value, 
        "T must be a descendant of hero"
    );

    using T::T;

    static unsigned int id()
    {
        return 0;
    }
};

template<typename T>
class hero_2
    : public T
{
public:
    static_assert(
        std::is_base_of<hero, T>::value, 
        "T must be a descendant of hero"
    );

    using T::T;

    static unsigned int id()
    {
        return 1;
    }
};

}
