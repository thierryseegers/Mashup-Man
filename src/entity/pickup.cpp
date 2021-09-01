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
    : pickup{std::make_unique<animated_sprite_t>(
                *magic_enum::enum_cast<resources::texture>(*configuration::values()["items"]["texture"].value<std::string>()),
                utility::to_intrect(*configuration::values()["items"]["coin"]["texture_rect"].as_array()),
                utility::to_vector2i(*configuration::values()["items"]["coin"]["frame_size"].as_array()),
                *configuration::values()["items"]["coin"]["n_frames"].value<std::size_t>(),
                sf::seconds(*configuration::values()["items"]["coin"]["duration"].value<float>()),
                *configuration::values()["items"]["coin"]["repeat"].value<bool>(),
                configuration::values()["items"]["coin"]["scale"].value_or<float>(1.f))}
{}

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
    : pickup{std::make_unique<sprite_t>(
                *magic_enum::enum_cast<resources::texture>(*configuration::values()["items"]["texture"].value<std::string>()),
                utility::to_intrect(*configuration::values()["items"]["mushroom"]["texture_rect"].as_array()),
                configuration::values()["items"]["mushroom"]["scale"].value_or<float>(1.f))}
{}

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
    : pickup{std::make_unique<animated_sprite_t>(
                *magic_enum::enum_cast<resources::texture>(*configuration::values()["items"]["texture"].value<std::string>()),
                utility::to_intrect(*configuration::values()["items"]["flower"]["texture_rect"].as_array()),
                utility::to_vector2i(*configuration::values()["items"]["flower"]["frame_size"].as_array()),
                *configuration::values()["items"]["flower"]["n_frames"].value<std::size_t>(),
                sf::seconds(*configuration::values()["items"]["flower"]["duration"].value<float>()),
                *configuration::values()["items"]["flower"]["repeat"].value<bool>(),
                configuration::values()["items"]["flower"]["scale"].value_or<float>(1.f))}
{}

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
