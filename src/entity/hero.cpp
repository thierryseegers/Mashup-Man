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
    , maze_{nullptr}
    , dead_{false}
    , immunity{sf::seconds(3)}
{}

direction hero::steering() const
{
    return steering_;
}

bool hero::dead() const
{
    return dead_;
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
    dead_ = true;
}

void hero::attack()
{}

void hero::update_self(
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
    else if(dead_)
    {
        commands.push(make_command(std::function{[=](layer::animations& layer, sf::Time const&)
        {
            std::unique_ptr<scene::node> d{tombstone()};
            d->setPosition(getPosition());
            layer.attach(std::move(d));

            remove = true;
        }}));
    }
    else
    {
    //         sf::Vector2i const position{(int)hero->getPosition().x, (int)hero->getPosition().y};
    // auto const heading = hero->heading();
    // auto const steering = hero->steering();

    // if(position.x % level::tile_size >= 8 && position.x % level::tile_size <= 12 && position.y % level::tile_size >= 8 && position.y % level::tile_size <= 12)
    // {
    //     // If the hero wants to change direction and he can, let him.
    //     if((steering != heading || hero->speed() == 0.f) &&
    //        ((steering == direction::right  && !utility::any_of((*maze_)[{position.x / level::tile_size + 1, position.y / level::tile_size}], '0', '1', '2', '3', 'd')) ||
    //         (steering == direction::left   && !utility::any_of((*maze_)[{position.x / level::tile_size - 1, position.y / level::tile_size}], '0', '1', '2', '3', 'd')) ||
    //         (steering == direction::down   && !utility::any_of((*maze_)[{position.x / level::tile_size, position.y / level::tile_size + 1}], '0', '1', '2', '3', 'd')) ||
    //         (steering == direction::up     && !utility::any_of((*maze_)[{position.x / level::tile_size, position.y / level::tile_size - 1}], '0', '1', '2', '3', 'd'))))
    //     {
    //         spdlog::info("Changing heading at coordinates [{}, {}] to [{}]", position.x, position.y, me::enum_name(steering));
    //         hero->head(steering);
    //         hero->throttle(1.f);
    //         hero->setPosition((position.x / level::tile_size) * level::tile_size + level::half_tile_size, (position.y / level::tile_size) * level::tile_size + level::half_tile_size);
    //     }
    //     // Else if the hero is crusing along and he's about to face a wall, stop him.
    //     else if(hero->speed() != 0.f &&
    //             !((heading == direction::left || heading == direction::right) && utility::any_of((*maze_)[{position.x / level::tile_size, position.y / level::tile_size}], 'p')) &&
    //             ((heading == direction::right    && utility::any_of((*maze_)[{position.x / level::tile_size + 1, position.y / level::tile_size}], '0', '1', '2', '3', 'd')) ||
    //              (heading == direction::left     && utility::any_of((*maze_)[{position.x / level::tile_size - 1, position.y / level::tile_size}], '0', '1', '2', '3', 'd')) ||
    //              (heading == direction::down     && utility::any_of((*maze_)[{position.x / level::tile_size, position.y / level::tile_size + 1}], '0', '1', '2', '3', 'd')) ||
    //              (heading == direction::up       && utility::any_of((*maze_)[{position.x / level::tile_size, position.y / level::tile_size - 1}], '0', '1', '2', '3', 'd'))))
    //     {
    //         spdlog::info("Hit a wall at coordinates [{}, {}] heading [{}]", position.x, position.y, me::enum_name(heading));
    //         hero->throttle(0.f);
    //     }
    //     // Else we let him cruise along.
    // }

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
