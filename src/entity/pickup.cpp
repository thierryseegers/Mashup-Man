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

coin::coin()
    : pickup{*magic_enum::enum_cast<resources::texture>(*configuration::values()["items"]["texture"].value<std::string>()),
             utility::to_intrect(*configuration::values()["items"]["coin"]["texture_rect"].as_array()),
             utility::to_vector2i(*configuration::values()["items"]["coin"]["frame_size"].as_array()),
             *configuration::values()["items"]["coin"]["n_frames"].value<std::size_t>(),
             sf::seconds(*configuration::values()["items"]["coin"]["duration"].value<float>()),
             *configuration::values()["items"]["coin"]["repeat"].value<bool>(),
             configuration::values()["items"]["coin"]["scale"].value_or<float>(1.f)}
{}

void coin::apply(
    brother& /*b*/) const
{
    // leader.repair(25);
}

mushroom::mushroom()
    : pickup{*magic_enum::enum_cast<resources::texture>(*configuration::values()["items"]["texture"].value<std::string>()),
             utility::to_intrect(*configuration::values()["items"]["mushroom"]["texture_rect"].as_array()),
             configuration::values()["items"]["mushroom"]["scale"].value_or<float>(1.f)}
{}

void mushroom::apply(
    brother& /*b*/) const
{
    // leader.repair(25);
}

flower::flower()
    : pickup{*magic_enum::enum_cast<resources::texture>(*configuration::values()["items"]["texture"].value<std::string>()),
             utility::to_intrect(*configuration::values()["items"]["flower"]["texture_rect"].as_array()),
             utility::to_vector2i(*configuration::values()["items"]["flower"]["frame_size"].as_array()),
             *configuration::values()["items"]["flower"]["n_frames"].value<std::size_t>(),
             sf::seconds(*configuration::values()["items"]["flower"]["duration"].value<float>()),
             *configuration::values()["items"]["flower"]["repeat"].value<bool>(),
             configuration::values()["items"]["flower"]["scale"].value_or<float>(1.f)}
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
