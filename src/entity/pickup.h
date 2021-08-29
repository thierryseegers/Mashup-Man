#pragma once

#include "entity/entity.h"
#include "entity/brother.h"
#include "resources.h"

#include <SFML/Graphics/Rect.hpp>

namespace entity::pickup
{

class pickup : public entity<>
{
public:
    pickup(
        resources::texture const& texture,
        sf::IntRect const& texture_rect);

    virtual void apply(
        brother& b) const = 0;
};

class mushroom : public pickup
{
public:
    mushroom();

    virtual void apply(
        brother& b) const override;
};

class flower : public pickup
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
