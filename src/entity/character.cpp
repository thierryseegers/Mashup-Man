#include "entity/character.h"

#include "resources.h"
#include "scene.h"
#include "utility.h"

#include <SFML/System.hpp>

#include <memory>

namespace entity
{

character::character(
    std::unique_ptr<sprite_t> sprite)
    : entity{std::move(sprite)}
{}

void character::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    // if(!life)
    // {
    //     remove = true;
    // }
    // bar->adjust((life * 100) / starting_life);

    entity::update_self(dt, commands);
}

}
