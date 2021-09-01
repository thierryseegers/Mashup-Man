#pragma once

#include "entity/entity.h"
#include "entity/brother.h"
#include "resources.h"
#include "sprite.h"

#include <SFML/Graphics/Rect.hpp>

#include <memory>

namespace entity::pickup
{

class pickup :
    public entity
{
public:
    virtual resources::sound_effect sound_effect() const = 0;

    virtual void apply(
        brother& b) const = 0;
};

class coin :
    public pickup
{
public:
    coin();

    virtual resources::sound_effect sound_effect() const override;

    virtual void apply(
        brother& b) const override;
};

class mushroom : 
    public pickup
{
public:
    mushroom();

    virtual resources::sound_effect sound_effect() const override;

    virtual void apply(
        brother& b) const override;
};

class flower : 
    public pickup
{
public:
    flower();

    virtual resources::sound_effect sound_effect() const override;

    virtual void apply(
        brother& b) const override;
};

// class increase_spread : public pickup
// {
// public:
//     increase_spread();

//     virtual void apply(
//         brother& leader) const override;
// };

// class increase_fire_rate : public pickup
// {
// public:
//     increase_fire_rate();

//     virtual void apply(
//         brother& leader) const override;
// };

}
