#pragma once

#include "entity/animation.h"
#include "entity/projectile.h"

#include <array>

namespace entity
{

class fireball
    : public projectile
{
public:
    fireball(
        direction const heading_);

private:
    void update_self(
        sf::Time const& dt,
        commands_t& commands);
    
    std::array<int, 5> bounce_offsets;
    decltype(bounce_offsets)::iterator bo;
};

class fizzle
    : public animation
{
public:
    fizzle();
};

}
