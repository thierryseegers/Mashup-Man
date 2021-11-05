#pragma once

#include "command.h"
#include "direction.h"
#include "entity/character.h"
#include "entity/power_up.h"
#include "maze.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

namespace entity
{

class hero
    : public friendly<character>
{
public:
    hero(
        sprite sprite_,
        int const max_speed = 0);

    // Whether the hero is dead. (Death doesn't always follow from a hit().)
    [[nodiscard]] bool dead() const;
    
    // The hero may be impervious to damage when recently spawned.
    [[nodiscard]] bool immune() const override;

    // The direction towards which the hero is steered by the player.
    [[nodiscard]] direction steering() const;

    // The still image to be used to represent this hero (e.g. on the lifeboard).
    virtual sprite default_still() = 0;

    // The default animation to be used to represent this hero (e.g. on the character select screen).
    virtual sprite default_animated() = 0;

    // The hero has been hit, be it by a ghost or a projectile.
    virtual void hit() override;

    // The hero's attack (which may depend on a power-up).
    virtual void attack();

    // Pick up a power-up.
    virtual void pick_up(
        power_up*) = 0;

    // Steer the hero in a certain direction.
    void steer(
        direction const d);

protected:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    // The entity that this hero "transforms" into when dead.
    virtual entity* tombstone() const = 0;

    direction steering_;

    maze *maze_;
    bool dead_;
    sf::Time immunity;  // Immunity timer.
};

}
