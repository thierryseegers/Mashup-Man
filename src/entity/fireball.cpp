#include "entity/fireball.h"

#include "animation.h"
#include "configuration.h"
#include "entity/projectile.h"
#include "layer.h"
#include "maze.h"
#include "resources.h"
#include "sprite.h"
#include "utility.h"

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
    , fizzling{false}
    , maze_{nullptr}
{}

void fireball::hit()
{
    fizzling = true;
}

void fireball::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if(!maze_)
    {
        commands.push(make_command(std::function{[=](maze& m, sf::Time const&)
        {
            maze_ = &m;
        }}));
    }
    else if(fizzling)
    {
        commands.push(make_command(std::function{[position = getPosition()](layer::animations& layer, sf::Time const&)
        {
            layer.attach<fizzle>()->setPosition(position);
        }}));

        remove = true;
    }
    else
    {
        // Check if it's hitting a wall or not.
        int const r = getPosition().y / level::tile_size, c = getPosition().x / level::tile_size;
        if(utility::any_of((*maze_)[{c, r}], '0', '1', '2', '3', 'p'))
        {
            // Bring it closer to the wall.
            nudge(7.f);

            hit();

            commands.push(make_command(+[](scene::sound_t& sound, sf::Time const&)
            {
                sound.play(resources::sound_effect::bump);
            }));
        }

        entity::update_self(dt, commands);
    }
}

}
