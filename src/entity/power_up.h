#pragma once

#include "entity/entity.h"
#include "resources.h"

namespace entity::power_up
{

struct power_up_picker;

class power_up
    : public entity
{
public:
    using entity::entity;

    virtual void pick_up(power_up_picker*);

    virtual resources::sound_effect sound_effect() const = 0;
};

class coin
    : public power_up
{
public:
    coin();

    virtual void pick_up(power_up_picker*) override;

    virtual resources::sound_effect sound_effect() const override;
};

class mushroom
    : public power_up
{
public:
    mushroom();

    virtual void pick_up(power_up_picker*) override;

    virtual resources::sound_effect sound_effect() const override;
};

class flower
    : public power_up
{
public:
    flower();

    virtual void pick_up(power_up_picker*) override;

    virtual resources::sound_effect sound_effect() const override;
};

struct power_up_picker
{
    virtual void pick_up(coin const*) {}

    virtual void pick_up(mushroom const*) {}

    virtual void pick_up(flower const*) {}
};

}
