#include "entity/character.h"

#include "resources.h"
#include "scene.h"
#include "utility.h"

#include <SFML/System.hpp>

#include <memory>

namespace entity
{

character::character(
    resources::texture const& texture,
    sf::IntRect const& texture_rect,
    float const& scale)
    : entity{texture, texture_rect, scale}
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
