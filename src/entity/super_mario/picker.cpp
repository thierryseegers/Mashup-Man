#include "entity/super_mario/picker.h"

#include "entity/brothers.h"
#include "entity/super_mario/power_up.h"

namespace entity::super_mario
{

brother_picker::brother_picker(
    brother* brother_)
    : brother_{brother_}
{}

void brother_picker::pick_up(
    power_up* pu)
{
    // picker::pick_up(pu);
}

void brother_picker::pick_up(
    coin const* c)
{
    brother_->pick_up(c);
}

void brother_picker::pick_up(
    mushroom const* m)
{
    brother_->pick_up(m);
}

void brother_picker::pick_up(
    flower const* f)
{
    brother_->pick_up(f);
}

}
