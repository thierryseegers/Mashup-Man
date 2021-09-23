#pragma once

#include "entity/entity.h"
#include "entity/hero.h"
#include "resources.h"
#include "sprite.h"

#include <SFML/Graphics/Rect.hpp>

#include <memory>

namespace entity::power_up
{

class power_up
    : public entity
{
public:
    using entity::entity;

    virtual resources::sound_effect sound_effect() const = 0;

    virtual void apply(
        hero& b) const = 0;
};

class coin
    : public power_up
{
public:
    coin();

    virtual resources::sound_effect sound_effect() const override;

    virtual void apply(
        hero& b) const override;
};

class mushroom
    : public power_up
{
public:
    mushroom();

    virtual resources::sound_effect sound_effect() const override;

    virtual void apply(
        hero& b) const override;
};

class flower
    : public power_up
{
public:
    flower();

    virtual resources::sound_effect sound_effect() const override;

    virtual void apply(
        hero& b) const override;
};

}
