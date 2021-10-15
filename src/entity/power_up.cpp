#include "entity/power_up.h"

#include "entity/super_mario/picker.h"

namespace entity::power_up
{

void power_up::picked_up(
    picker*)
{
    remove = true;
}

void power_up::picked_up(
    super_mario::brother_picker* bp)
{
    picked_up(static_cast<picker*>(bp));
}

}
