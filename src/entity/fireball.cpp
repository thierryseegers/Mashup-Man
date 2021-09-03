#include "entity/fireball.h"

#include "configuration.h"
#include "entity/projectile.h"
#include "resources.h"
#include "sprite.h"

namespace entity
{

fireball::fireball(
    direction const heading_)
    : projectile{
        sprite{
            resources::texture::items,
            {{96, 144, 8, 8}, {104, 144, 8, 8}, {96, 152, 8, 8}, {104, 152, 8, 8}},
            sf::seconds(0.2f),
            sprite::repeat::loop},
        *configuration::values()["brothers"]["fireball"]["speed"].value<int>(),
        heading_}
{}

}