#pragma once

#include "command.h"
#include "entity/projectile.h"
#include "maze.h"

#include <SFML/System.hpp>

namespace entity::super_mario
{

class fireball
    : public friendly<projectile>
{
public:
    fireball(
        unsigned int const player_id,
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

class hammer
    : public hostile<projectile>
{
public:
    hammer(
        unsigned int const player_id,
        direction const heading_);

    virtual ~hammer() = default;

    virtual void hit() override;

private:
    void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    sf::Time const rotation_time = sf::milliseconds(250);
    sf::Time rotation_timer;
    float rotation_degrees;

    sf::Vector2f velocity;
};

}
