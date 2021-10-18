#include "entity/super_mario/power_up.h"

#include "configuration.h"
#include "entity/super_mario/picker.h"
#include "resources.h"
#include "sprite.h"

#include <SFML/Graphics.hpp>

#include <vector>

namespace entity::super_mario
{

coin::coin()
    : super_mario_power_up{
        sprite{
            resources::texture::items,
            std::vector<sf::IntRect>{{0, 96, 16, 16}, {16, 96, 16, 16}, {32, 96, 16, 16}, {48, 96, 16, 16}},
            sf::seconds(1.f),
            sprite::repeat::loop,
            configuration::values()["items"]["coin"]["scale"].value_or<float>(1.f)}}
{}

void coin::picked_up(brother_picker* picker)
{
    picker->pick_up(this);
    power_up::picked_up(picker);
}

resources::sound_effect coin::sound_effect() const
{
    return resources::sound_effect::collect_coin;
}

mushroom::mushroom()
    : super_mario_power_up{
        sprite{
            resources::texture::items,
            sf::IntRect{0, 0, 16, 16},
            configuration::values()["items"]["mushroom"]["scale"].value_or<float>(1.f)}}
{}

void mushroom::picked_up(brother_picker* picker)
{
    picker->pick_up(this);
    power_up::picked_up(picker);
}

resources::sound_effect mushroom::sound_effect() const
{
    return resources::sound_effect::collect_powerup;
}

flower::flower()
    : super_mario_power_up{
        sprite{
            resources::texture::items,
            std::vector<sf::IntRect>{{0, 32, 16, 16}, {16, 32, 16, 16}, {32, 32, 16, 16}, {48, 32, 16, 16}},
            sf::seconds(1.f),
            sprite::repeat::loop,
            configuration::values()["items"]["flower"]["scale"].value_or<float>(1.f)}}
{}

void flower::picked_up(brother_picker* picker)
{
    picker->pick_up(this);
    power_up::picked_up(picker);
}

resources::sound_effect flower::sound_effect() const
{
    return resources::sound_effect::collect_powerup;
}

}
