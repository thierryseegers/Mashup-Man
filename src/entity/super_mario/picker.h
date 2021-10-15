#pragma once

#include "entity/picker.h"
#include "entity/brothers.h"
#include "entity/super_mario/power_up.h"

namespace entity::power_up::super_mario
{

class brother_picker
    : public picker
{
    brother *brother_;

public:
    brother_picker(
        brother* brother_);

    virtual void pick_up(
        power_up*) override;

    void pick_up(
        super_mario::coin const*);

    void pick_up(
        super_mario::mushroom const*);

    void pick_up(
        super_mario::flower const*);
};

}
