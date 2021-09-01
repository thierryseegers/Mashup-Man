#include "pickup.h"

#include "configuration.h"
#include "entity/brother.h"
#include "resources.h"
#include "sprite.h"
#include "utility.h"

#include <magic_enum.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <memory>
#include <string>

namespace entity::pickup
{

coin::coin()
{
    sprite = std::make_unique<animated_sprite_t>(
                resources::texture::items,
                std::vector<sf::IntRect>{{0, 96, 16, 16}, {16, 96, 16, 16}, {32, 96, 16, 16}, {48, 96, 16, 16}},
                sf::seconds(1.f),
                animated_sprite_t::repeat::loop,
                configuration::values()["items"]["coin"]["scale"].value_or<float>(1.f));
}

resources::sound_effect coin::sound_effect() const
{
    return resources::sound_effect::collect_coin;
}

void coin::apply(
    brother& /*b*/) const
{
    // leader.repair(25);
}

mushroom::mushroom()
{
    sprite = std::make_unique<sprite_t>(
                *magic_enum::enum_cast<resources::texture>(*configuration::values()["items"]["texture"].value<std::string>()),
                utility::to_intrect(*configuration::values()["items"]["mushroom"]["texture_rect"].as_array()),
                configuration::values()["items"]["mushroom"]["scale"].value_or<float>(1.f));
}

resources::sound_effect mushroom::sound_effect() const
{
    return resources::sound_effect::collect_powerup;
}

void mushroom::apply(
    brother& /*b*/) const
{
    // leader.repair(25);
}

flower::flower()
{
    sprite = std::make_unique<animated_sprite_t>(
                resources::texture::items,
                std::vector<sf::IntRect>{{0, 32, 16, 16}, {16, 32, 16, 16}, {32, 32, 16, 16}, {48, 32, 16, 16}},
                sf::seconds(1.f),
                animated_sprite_t::repeat::loop,
                configuration::values()["items"]["flower"]["scale"].value_or<float>(1.f));
}

resources::sound_effect flower::sound_effect() const
{
    return resources::sound_effect::collect_powerup;
}

void flower::apply(
    brother& /*b*/) const
{
    // leader.collect_missile(3);
}

}
