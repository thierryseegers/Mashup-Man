#pragma once

#include "entity/enemy.h"

#include <string_view>

namespace entity::super_mario
{

class goomba
    : public follower
{
public:
    goomba();

    virtual std::string_view name() const override;

    virtual void update_sprite() override;
};

class koopa
    : public ahead
{
public:
    koopa();

    virtual std::string_view name() const override;

    virtual void update_sprite() override;
};

class beetle
    : public axis
{
public:
    beetle();

    virtual std::string_view name() const override;

    virtual void update_sprite() override;
};

class hammer_brother
    : public skittish
{
public:
    hammer_brother();

    virtual std::string_view name() const override;

    virtual void update_sprite() override;

private:
    void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    sf::Time const throw_time = sf::seconds(4);
    sf::Time throw_timer;
};

}
