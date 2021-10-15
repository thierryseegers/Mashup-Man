#pragma once

namespace entity::power_up
{

class power_up;

class picker
{
public:
    virtual ~picker() = default;

    virtual void pick_up(power_up*) = 0;
};

}
