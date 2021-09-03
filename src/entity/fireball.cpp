#include "entity/fireball.h"

#include "entity/entity.h"
#include "resources.h"
#include "sprite.h"

namespace entity
{

fireball::fireball()
    : entity{
        sprite{
            resources::texture::items,
            {{96, 144, 8, 8}, {104, 144, 8, 8}, {96, 152, 8, 8}, {104, 152, 8, 8}},
            sf::seconds(0.2f),
            sprite::repeat::loop}}
{}

}