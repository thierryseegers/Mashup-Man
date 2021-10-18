#pragma once

#include "entity/power_up.h"
#include "resources.h"

namespace entity::super_mario
{

class brother_picker;

class super_mario_power_up
    : public power_up
{
public:
    using power_up::power_up;

    virtual ~super_mario_power_up() = default;

    virtual void picked_up(brother_picker*) = 0;
};

class coin
    : public super_mario_power_up
{
public:
    coin();

    virtual void picked_up(brother_picker*) override;

    virtual resources::sound_effect sound_effect() const override;
};

class mushroom
    : public super_mario_power_up
{
public:
    mushroom();

    virtual void picked_up(brother_picker*) override;

    virtual resources::sound_effect sound_effect() const override;
};

class flower
    : public super_mario_power_up
{
public:
    flower();

    virtual void picked_up(brother_picker*) override;

    virtual resources::sound_effect sound_effect() const override;
};

}
