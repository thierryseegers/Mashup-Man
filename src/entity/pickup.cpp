#include "pickup.h"

#include "configuration.h"
#include "entity/brother.h"
#include "resources.h"
#include "utility.h"

#include <magic_enum.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <string>

namespace entity::pickup
{

pickup::pickup(
    resources::texture const& texture,
    sf::IntRect const& texture_rect)
    : entity{texture, texture_rect}
{
    // utility::center_origin(sprite);
}

mushroom::mushroom()
    : pickup{*magic_enum::enum_cast<resources::texture>(*configuration::values()["items"]["texture"].value<std::string>()),
             utility::to_intrect(*configuration::values()["items"]["mushroom"]["texture_rect"].as_array())}
{}

void mushroom::apply(
    brother& /*b*/) const
{
    // leader.repair(25);
}

flower::flower()
    : pickup{*magic_enum::enum_cast<resources::texture>(*configuration::values()["items"]["texture"].value<std::string>()),
             utility::to_intrect(*configuration::values()["items"]["flower"]["texture_rect"].as_array())}
{}

void flower::apply(
    brother& /*b*/) const
{
    // leader.collect_missile(3);
}

// increase_spread::increase_spread()
//     : pickup{*magic_enum::enum_cast<resources::texture>(*configuration::values()["pickup"]["texture"].value<std::string>()),
//              utility::to_intrect(*configuration::values()["pickup"]["fire_spread"]["texture_rect"].as_array())}
// {}

// void increase_spread::apply(
//     brother& leader) const
// {
//     leader.increase_bullet_spread();
// }

// increase_fire_rate::increase_fire_rate()
//     : pickup{*magic_enum::enum_cast<resources::texture>(*configuration::values()["pickup"]["texture"].value<std::string>()),
//              utility::to_intrect(*configuration::values()["pickup"]["fire_rate"]["texture_rect"].as_array())}
// {}

// void increase_fire_rate::apply(
//     brother& leader) const
// {
//     leader.increase_fire_rate();
// }

}
