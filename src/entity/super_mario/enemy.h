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
    : public ahead
{
public:
    beetle();

    virtual std::string_view name() const override;

    virtual void update_sprite() override;
};

}
