#pragma once

#include "entity/entity.h"
#include "resources.h"

namespace entity
{

class picker;

namespace super_mario
{

class brother_picker;

}

class power_up
    : public entity
{
public:
    using entity::entity;

    void picked_up(
        picker*);

    virtual void picked_up(
        super_mario::brother_picker*);

    virtual resources::sound_effect sound_effect() const = 0;
};

}
