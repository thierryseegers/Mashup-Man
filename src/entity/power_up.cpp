#include "entity/power_up.h"

#include "configuration.h"
#include "entity/hero.h"
#include "resources.h"
#include "sprite.h"
#include "utility.h"

#include <magic_enum.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <memory>
#include <string>

namespace entity::power_up
{

coin::coin()
    : power_up{
        sprite{
            resources::texture::items,
            std::vector<sf::IntRect>{{0, 96, 16, 16}, {16, 96, 16, 16}, {32, 96, 16, 16}, {48, 96, 16, 16}},
            sf::seconds(1.f),
            sprite::repeat::loop,
            configuration::values()["items"]["coin"]["scale"].value_or<float>(1.f)}}
{}

resources::sound_effect coin::sound_effect() const
{
    return resources::sound_effect::collect_coin;
}

void coin::apply(
    hero& /*b*/) const
{
    // leader.repair(25);
}

mushroom::mushroom()
    : power_up{
        sprite{
            resources::texture::items,
            sf::IntRect{0, 0, 16, 16},
            configuration::values()["items"]["mushroom"]["scale"].value_or<float>(1.f)}}
{}

resources::sound_effect mushroom::sound_effect() const
{
    return resources::sound_effect::collect_powerup;
}

void mushroom::apply(
    hero& hero_) const
{
    hero_.consume_mushroom();
}

flower::flower()
    : power_up{
        sprite{
            resources::texture::items,
            std::vector<sf::IntRect>{{0, 32, 16, 16}, {16, 32, 16, 16}, {32, 32, 16, 16}, {48, 32, 16, 16}},
            sf::seconds(1.f),
            sprite::repeat::loop,
            configuration::values()["items"]["flower"]["scale"].value_or<float>(1.f)}}
{}

resources::sound_effect flower::sound_effect() const
{
    return resources::sound_effect::collect_powerup;
}

void flower::apply(
    hero& hero_) const
{
    hero_.consume_flower();
}

}
