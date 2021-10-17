#include "entity/enemy.h"

#include "configuration.h"
#include "direction.h"
#include "entity/character.h"
#include "entity/hero.h"
#include "level.h"
#include "resources.h"
#include "scene.h"
#include "sprite.h"
#include "tomlpp.h"
#include "utility.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>

#include <cmath>
#include <functional>
#include <map>
#include <memory>
#include <string_view>
#include <vector>

namespace entity
{

namespace me = magic_enum;

sf::Vector2i random_level_corner()
{
    // Pick a random corner area as a target.
    return {1 + utility::random(1) * ((int)level::width - 2 - 1),
            2 + utility::random(1) * ((int)level::height - 3 - 2)};
}

sf::Vector2i random_home_corner(
    sf::IntRect const& home)
{
    return {home.left + utility::random(1) * (home.width - 1),
            home.top + utility::random(1) * (home.height - 1)};
}

sf::Vector2i to_maze_coordinates(
    sf::Vector2f const& position)
{
    return {(int)position.x / level::tile_size, (int)position.y / level::tile_size};
}

enemy::enemy(
    animated_sprite_rects_f const animated_sprite_rects,
    dead_sprite_rect_f const dead_sprite_rect,
    float const scale_factor,
    int const max_speed,
    direction const heading_)
    : hostile<character>{
        sprite{
            resources::texture::enemies,
            animated_sprite_rects(mode::confined),
            sf::seconds(0.25f),
            sprite::repeat::loop,
            scale_factor
            }
        , max_speed
        , heading_}
    , animated_sprite_rects{animated_sprite_rects}
    , dead_sprite_rect{dead_sprite_rect}
    , current_mode_{mode::confined}
    , requested_mode_{mode::scatter}
    , maze_{nullptr}
    , healed{true}
    , target_{0, 0}
    , confinement{sf::seconds(10)}
{}

void enemy::hit()
{
    behave(mode::dead);

    update_sprite();
}

bool enemy::immune() const
{
    return current_mode_ == mode::dead;
}

void enemy::behave(
    mode const requested_mode)
{
    if(current_mode_ == requested_mode ||
       (current_mode_ == mode::dead && !healed) ||
       (current_mode_ == mode::confined && confinement > sf::Time::Zero))
    {
        requested_mode_ = requested_mode;
        return;
    }

    spdlog::info("{} changing mode from {} to {}.", name(), me::enum_name(current_mode_), me::enum_name(requested_mode));

    // As per the Pac-Man rule, ghosts reverse direction when switching from `chase` or `scatter`.
    if(current_mode_ == mode::chase ||
       current_mode_ == mode::scatter)
    {
        head(~heading());
    }

    current_mode_ = requested_mode;

    switch(current_mode_)
    {
        case mode::confined:
            assert(!"Do not manually change behavior to 'confined'. This mode is set form the start and never reoccurs.");
            break;
        case mode::chase:
            throttle(1.f);
            break;
        case mode::dead:
            healed = false;
            target_ = random_home_corner(maze_->ghost_house());
            throttle(2.f);
            break;
        case mode::frightened:
            break;
        case mode::scatter:
            target_ = random_level_corner();
            spdlog::info("{} targeting random corner [{}, {}].", name(), target_.x, target_.y);
            throttle(1.f);
            break;
    }

    update_sprite();
}

void enemy::update_sprite()
{
    if(current_mode_ == mode::dead)
    {
        sprite_.still(dead_sprite_rect());
    }
    else
    {
        sprite_.animate(animated_sprite_rects(current_mode_), sf::seconds(0.25f), sprite::repeat::loop);
    }

    character::update_sprite();
}

void enemy::update_self(
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
    else
    {
        sf::Vector2f const position = getPosition();
        sf::Vector2f const future_position = position + to_vector(heading_) * (max_speed * throttle_) * dt.asSeconds();

        // If the position delta between now and then crosses the middle of a tile...
        if((future_position.x > position.x && fmod(position.x, level::tile_size) <= level::half_tile_size && fmod(future_position.x, level::tile_size) > level::half_tile_size) ||
           (future_position.x < position.x && fmod(position.x, level::tile_size) >= level::half_tile_size && fmod(future_position.x, level::tile_size) < level::half_tile_size) ||
           (future_position.y > position.y && fmod(position.y, level::tile_size) <= level::half_tile_size && fmod(future_position.y, level::tile_size) > level::half_tile_size) ||
           (future_position.y < position.y && fmod(position.y, level::tile_size) >= level::half_tile_size && fmod(future_position.y, level::tile_size) < level::half_tile_size))
        {
            std::set<direction> paths;
            auto const around = maze_->around({(int)position.x / level::tile_size, (int)position.y / level::tile_size});
            if(heading() != direction::left && utility::any_of(around.at(direction::right), maze::path, maze::door))
            {
                paths.insert(direction::right);
            }
            if(heading() != direction::right && utility::any_of(around.at(direction::left), maze::path, maze::door))
            {
                paths.insert(direction::left);
            }
            if(heading() != direction::up && utility::any_of(around.at(direction::down), maze::path, maze::door))
            {
                paths.insert(direction::down);
            }
            if(heading() != direction::down && utility::any_of(around.at(direction::up), maze::path, maze::door))
            {
                paths.insert(direction::up);
            }

            // In case it's in a pipe...
            if(heading() == direction::left && around.at(direction::left) == maze::pipe)
            {
                paths.insert(direction::right);
            }
            else if(heading() == direction::right && around.at(direction::right) == maze::pipe)
            {
                paths.insert(direction::left);
            }

            // If it is at an intersection, ask it for a direction.
            if(paths.size() >= 2)
            {
                // Pick and adjust heading given current target.
                sf::Vector2i const start{(int)position.x / level::tile_size, (int)position.y / level::tile_size};
                auto const r = maze_->route(start, target_);

                // Head towards the best route but only if it's not backtracking because that is not allowed.
                if(r != ~heading() && r != direction::none)
                {
                    head(r);
                }
                else
                {
                    head(*paths.begin());
                }

                spdlog::info("{} decided to go {}", name(), me::enum_name(heading()));
            }
            // Else, if it hit a wall, follow along the path.
            else if(*paths.begin() != heading())
            {
                head(*paths.begin());

                spdlog::info("{} is turning {}", name(), me::enum_name(heading()));
            }
            // Else, let it cruise along.
        }
    }

    if(current_mode_ == mode::confined)
    {
        if(!maze_)
        {}
        else if((confinement -= dt) <= sf::Time::Zero ||
                !maze_->ghost_house().contains(to_maze_coordinates(getPosition())))
        {
            confinement = sf::Time::Zero;

            behave(requested_mode_);
        }
        else if(target_ == sf::Vector2i{0, 0} ||
                to_maze_coordinates(getPosition()) == target_)
        {
            target_ = random_home_corner(maze_->ghost_house());
        }
    }
    else if(current_mode_ == mode::dead &&
            maze_->ghost_house().contains(to_maze_coordinates(getPosition())))
    {
        healed = true;

        behave(requested_mode_);
    }

    character::update_self(dt, commands);
}

void follower::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    commands.push(make_command(std::function{[this](layer::characters& characters, sf::Time const&)
    {
        if(current_mode_ == mode::chase)
        {
            // Find the closest hero and target it.
            std::vector<sf::Vector2f> heroes_positions;

            for(auto* const character : characters.children())
            {
                if(auto const* h = dynamic_cast<hero const*>(character))
                {
                    heroes_positions.push_back(h->getPosition());
                }
            }

            if(heroes_positions.size())
            {
                auto const closest = std::min_element(heroes_positions.begin(), heroes_positions.end(), [this](auto const& p1, auto const& p2)
                {
                    return utility::length(getPosition() - p1) < utility::length(getPosition() - p2);
                });

                target_ = to_maze_coordinates(*closest);
            }
        }
    }}));

    enemy::update_self(dt, commands);
}

}
