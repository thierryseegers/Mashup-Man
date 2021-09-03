#include "entity/character.h"

#include "resources.h"
#include "scene.h"
#include "sprite.h"
#include "utility.h"

#include <SFML/System.hpp>

#include <memory>

namespace entity
{

character::character(
    sprite sprite_,
    float const speed,
    direction const facing_,
    direction const heading_)
    : entity{
        sprite_,
        speed,
        heading_}
    , facing_{facing_}
{}

direction character::facing() const
{
    return facing_;
}

void character::face(
    direction const d)
{
    facing_ = d;
}

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
