#include "entity/fireball.h"

#include "animation.h"
#include "configuration.h"
#include "entity/projectile.h"
#include "layer.h"
#include "resources.h"
#include "sprite.h"

#include <magic_enum.hpp>

namespace entity
{

class fizzle
    : public animation
{
public:
    fizzle()
    : animation{
        resources::texture::items,
        {{112, 144, 16, 16}, {112, 160, 16, 16}, {112, 176, 16, 16}},
        sf::seconds(0.1f)}
    {}
};

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

void fireball::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if(remove)
    {
        commands.push(make_command(std::function{[position = getPosition()](layer::animations& layer, sf::Time const&)
        {
            layer.attach<fizzle>()->setPosition(position);
        }}));
    }
    else
    {
        entity::update_self(dt, commands);
    }
}

}
