#pragma once

#include "entity/enemy.h"

#include <string_view>

namespace entity::super_mario
{

class goomba
    : public chaser
{
public:
    goomba();

    virtual std::string_view name() const override;
};

class koopa
    : public chaser
{
public:
    koopa();

    virtual std::string_view name() const override;

};

}
