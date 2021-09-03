#include "entity/animation.h"

#include "resources.h"
#include "sprite.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

#include <vector>

namespace entity
{

animation::animation(
    resources::texture const& texture,
    std::vector<sf::IntRect> const& texture_rects,
    sf::Time const duration,
    float const scale_factor)
    : entity{
        sprite{
            texture,
            texture_rects,
            duration,
            sprite::repeat::none,
            scale_factor}}
    , countdown{duration}
{}

void animation::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if((countdown -= dt) <= sf::Time::Zero)
    {
        remove = true;
    }
    else
    {
        entity::update_self(dt, commands);
    }
}

}
