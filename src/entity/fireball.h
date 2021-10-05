#pragma once

#include "command.h"
#include "entity/animation.h"
#include "entity/projectile.h"
#include "maze.h"

#include <SFML/System.hpp>

namespace entity
{

class fireball
    : public projectile
{
public:
    fireball(
        direction const heading_);

    virtual ~fireball() = default;

private:
    void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    bool fizzling;
    maze *maze_;
};

}
