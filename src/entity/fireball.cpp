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
    , hit_{false}
{}

void fireball::hit()
{
    hit_ = true;
}

void fireball::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if(hit_)
    {
        commands.push(make_command<layer::projectiles>([=](layer::projectiles& layer, sf::Time const&)
        {
            layer.attach<fizzle>()->setPosition(getPosition());
        }));

        remove = true;
    }
    else
    {
        static auto const directions = []
        {
            return std::array<sf::Vector2f, magic_enum::enum_count<direction>()>{
                    sf::Vector2f{0.f, -1.f},
                    sf::Vector2f{0.f, 1.f},
                    sf::Vector2f{-1.f, 0.f},
                    sf::Vector2f{1.f, 0.f}};
        }();

        sf::Transformable::move(directions[magic_enum::enum_integer(heading_)] * (max_speed * throttle_) * dt.asSeconds());
    }

    sprite_.update(dt, commands);
}

}
