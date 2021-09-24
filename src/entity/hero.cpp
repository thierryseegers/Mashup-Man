#include "entity/hero.h"

#include "command.h"
#include "configuration.h"
#include "entity/character.h"
#include "entity/entity.h"
#include "layer.h"
#include "level.h"
#include "resources.h"
#include "scene.h"
#include "utility.h"

#include <magic_enum.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

#include <algorithm>
#include <array>

namespace entity
{

hero::hero(
    sprite sprite_,
    int const max_speed)
    : friendly<character>{
        sprite_,
        max_speed}
    , immunity{sf::seconds(3)}
{}

direction hero::steering() const
{
    return steering_;
}

bool hero::immune() const
{
    return immunity > sf::Time::Zero;
}

void hero::steer(
    direction const d)
{
    steering_ = d;
}

void hero::hit()
{
    remove = true;
}

void hero::attack()
{}

void hero::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if(remove)
    {
        auto d = dead();
        d->setPosition(getPosition());

        commands.push(make_command<layer::animations>([d](layer::animations& layer, sf::Time const&)
        {
            std::unique_ptr<scene::node> u{d};
            layer.attach(std::move(u));
        }));
    }
    else
    {
        // Flash transparency to denote temporary immunity.
        if(immunity > sf::Time::Zero)
        {
            immunity = std::max(sf::Time::Zero, immunity - dt);

            if((int)(immunity.asSeconds() * 30) % 2)
            {
                sprite_.set_color(sf::Color::Black);
            }
            else
            {
                sprite_.set_color({255, 255, 255, (255 * 3) / 4});
            }
        }
        else
        {
            sprite_.set_color(sf::Color::White);
        }

        character::update_self(dt, commands);
    }
}

}
