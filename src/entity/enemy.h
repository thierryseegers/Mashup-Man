#pragma once

#include "command.h"
#include "entity/character.h"
#include "entity/entity.h"
// #include "entity/flight.h"
#include "resources.h"
#include "scene.h"
#include "sprite.h"
#include "utility.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

#include <memory>
#include <vector>

namespace entity
{

class enemy
    : public hostile<character>
{
public:
    using hostile<character>::hostile;

    virtual ~enemy() = default;

protected:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    // virtual void attack(
    //     scene::projectiles& layer) const = 0;

    // int const speed;

    // flight::pattern const pattern;
    // utility::cyclic_iterator<decltype(pattern)> current;

    // float travelled;

    // float const attack_rate;
    // sf::Time attack_countdown;
};

class goomba
    : public enemy
{
public:
    goomba();

protected:
    // virtual void attack(
    //     scene::projectiles& layer) const override;
};

// class raptor : public enemy
// {
// public:
//     raptor();

// protected:
//     virtual void attack(
//         scene::projectiles& layer) const override;
// };

}