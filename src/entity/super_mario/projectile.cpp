#include "entity/super_mario/projectile.h"

#include "configuration.h"
#include "entity/animation.h"
#include "entity/projectile.h"
#include "layer.h"
#include "maze.h"
#include "resources.h"
#include "sprite.h"
#include "utility.h"

namespace entity::super_mario
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
    : friendly<projectile>{
        sprite{
            resources::texture::items,
            {{96, 144, 8, 8}, {104, 144, 8, 8}, {96, 152, 8, 8}, {104, 152, 8, 8}},
            sf::seconds(0.2f),
            sprite::repeat::loop},
        *configuration::values()["brothers"]["fireball"]["speed"].value<int>(),
        heading_}
    , fizzled{false}
    , maze_{nullptr}
{}

void fireball::hit()
{
    fizzled = true;
}

void fireball::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if(!maze_)
    {
        commands.push(make_command(std::function{[this](maze& m, sf::Time const&)
        {
            maze_ = &m;
        }}));
    }
    else if(fizzled)
    {
        commands.push(make_command(std::function{[this](layer::animations& layer, sf::Time const&)
        {
            layer.attach<fizzle>()->setPosition(getPosition());

            remove = true;
        }}));
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

hammer::hammer(
    direction const heading_)
    : hostile<projectile>{
        sprite{
            resources::texture::enemies,
            {486, 12, 8, 16}
        },
        0,
        heading_}
    , rotation_timer{rotation_time / 4.f}
    , rotation_degrees{0.}
{
    switch(heading_)
    {
        case direction::down:
        default:
            velocity = {0.f, 32 * 5.f};
            break;
        case direction::left:
            velocity = {32 * -3.f, 32 * -5.f};
            break;
        case direction::right:
            velocity = {32 * 3.f, 32 * -5.f};
            break;
        case direction::up:
            velocity = {0.f, 32 * -5.f};
            break;
    }
}

void hammer::hit()
{
    remove = true;
}

void hammer::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if((rotation_timer -= dt) <= sf::Time::Zero)
    {
        rotation_timer += rotation_time / 4.f;

        sprite_.set_rotation(rotation_degrees = fmod(rotation_degrees + 90.f, 360.f));
    }

    velocity += sf::Vector2f{0.f, 32 * 9.8f} * dt.asSeconds();
    move(velocity * dt.asSeconds());
}

}
