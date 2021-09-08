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

    virtual ~fireball() = default;

private:
    void update_self(
        sf::Time const& dt,
        commands_t& commands) override;
};

}
