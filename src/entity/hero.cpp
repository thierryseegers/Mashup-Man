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
#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>

namespace entity
{

namespace me = magic_enum;

hero::hero(
    sprite sprite_,
    int const max_speed)
    : friendly<character>{
        sprite_,
        max_speed}
    , steering_{direction::none}
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
        commands.push(make_command(std::function{[this](maze& m, sf::Time const&)
        {
            maze_ = &m;
        }}));
    }
    else if(dead_)
    {
        commands.push(make_command(std::function{[this](layer::animations& layer, sf::Time const&)
        {
            std::unique_ptr<scene::node> d{tombstone()};
            d->setPosition(getPosition());
            layer.attach(std::move(d));

            remove = true;
        }}));
    }
    else
    {
        sf::Vector2f const position = getPosition();
        sf::Vector2f const future_position = position + to_vector2f(heading_) * (max_speed * 1.0f) * dt.asSeconds();

        // If the position delta between now and then crosses the middle of a tile...
        if((future_position.x > position.x && fmod(position.x, level::tile_size) <= level::half_tile_size && fmod(future_position.x, level::tile_size) > level::half_tile_size) ||
           (future_position.x < position.x && fmod(position.x, level::tile_size) >= level::half_tile_size && fmod(future_position.x, level::tile_size) < level::half_tile_size) ||
           (future_position.y > position.y && fmod(position.y, level::tile_size) <= level::half_tile_size && fmod(future_position.y, level::tile_size) > level::half_tile_size) ||
           (future_position.y < position.y && fmod(position.y, level::tile_size) >= level::half_tile_size && fmod(future_position.y, level::tile_size) < level::half_tile_size))
        {
            // If the hero wants to change direction and he can, let him.
            auto const around = maze_->around({(int)position.x / level::tile_size, (int)position.y / level::tile_size});
            if(steering() != direction::none &&
               (steering() != heading() || speed() == 0.f) &&
               !utility::any_of(maze::to_structure(around.at(steering())), maze::structure::wall, maze::structure::door))
            {
                spdlog::info("Changing heading at coordinates [{}, {}] to [{}]", position.x, position.y, me::enum_name(steering()));
                head(steering());
                throttle(1.f);
                setPosition(((int)position.x / level::tile_size) * level::tile_size + level::half_tile_size, ((int)position.y / level::tile_size) * level::tile_size + level::half_tile_size);
            }
            // Else if the hero is crusing along and he's about to face a wall, stop him.
            else if(speed() != 0.f &&
                    utility::any_of(maze::to_structure(around.at(heading())), maze::structure::wall, maze::structure::door))
            {
                spdlog::info("Hit a wall at coordinates [{}, {}] heading [{}]", position.x, position.y, me::enum_name(heading()));
                throttle(0.f);
            }
            // Else we let him cruise along.
        }

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
