#pragma once

#include "command.h"
#include "entity/projectile.h"
#include "maze.h"

#include <SFML/System.hpp>

namespace entity
{

class fireball
    : public friendly<projectile>
{
public:
    fireball(
        direction const heading_);

    virtual ~fireball() = default;

    virtual void hit() override;

private:
    void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    bool fizzled;
    maze *maze_;
};

}
