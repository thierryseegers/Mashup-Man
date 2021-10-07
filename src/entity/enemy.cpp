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

sf::Vector2f random_level_corner()
{
    // Pick a random corner area as a target.
    static size_t const x[2] = {1ul, level::width - 1 - 1};
    static size_t const y[2] = {2ul, level::height - 2 - 1};

    float const c = x[utility::random(1)];
    float const r = y[utility::random(1)];

    return {c * level::tile_size + level::half_tile_size, r * level::tile_size + level::half_tile_size};
}

sf::Vector2f random_home_corner(
    sf::FloatRect const& home)
{
    return {home.left + utility::random(1) * (home.width - level::tile_size) + level::half_tile_size,
            home.top + utility::random(1) * (home.height - level::tile_size) + level::half_tile_size};
}

bool contains(
    sf::FloatRect const& a,
    sf::FloatRect const& b)
{
    return b.left >= a.left && b.top >= a.top && (b.left + b.width <= a.left + a.width) && (b.top + b.height <= a.top + a.height);
}

enemy::enemy(
    animated_sprite_rects_f const animated_sprite_rects,
    dead_sprite_rect_f const dead_sprite_rect,
    float const scale_factor,
    sf::FloatRect const& home,
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
    , home{home}
    , healed{true}
    , target_{random_home_corner(home)}
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
       (current_mode_ == mode::confined && confinement >= sf::Time::Zero && home.contains(getPosition())))
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
            target_ = {home.left + home.width / 2.f, home.top + home.height / 2.f};
            throttle(2.f);
            break;
        case mode::frightened:
            break;
        case mode::scatter:
            target_ = random_level_corner();
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
        commands.push(make_command(std::function{[=](maze& m, sf::Time const&)
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
                sf::Vector2i const goal{(int)target_.x / level::tile_size, (int)target_.y / level::tile_size};
                auto const r = maze_->route(start, goal);

                // Head towards the best route but only if it's not backtracking because that is not allowed.
                if(r != ~heading())
                {
                    head(r);
                }
                else
                {
                    head(*paths.begin());
                }

                // Adjust position knowing where it's heading.
                // switch(heading_)
                // {
                //     case direction::up:
                //         setPosition(position.x % position.x % level::tile_size + level::half_tile_size, getPosition().y + level::half_tile_size - position.x % level::tile_size);
                //         break;
                //     case direction::down:
                //         setPosition(position.x % position.x % level::tile_size + level::half_tile_size, getPosition().y - (level::half_tile_size - position.x % level::tile_size));
                //         break;
                //     case direction::left:
                //         setPosition(position.x % position.x % level::tile_size + level::half_tile_size, getPosition().y + level::half_tile_size - position.x % level::tile_size);
                //         break;
                //     case direction::right:
                //         setPosition(position.x % position.x % level::tile_size + level::half_tile_size, getPosition().y + level::half_tile_size - position.x % level::tile_size);
                //         break;
                // }
                // Nudge it along so it doesn't get to redecide immediately...
                // nudge(2.f);

                spdlog::info("{} decided to go {}", name(), me::enum_name(heading()));
            }
            // Else, if it hit a wall, follow along the path.
            else if(*paths.begin() != heading())
            {
                head(*paths.begin());

                // Nudge it along so it doesn't get to redecide immediately...
                // nudge(2.f);

                spdlog::info("{} is turning {}", name(), me::enum_name(heading()));
            }
            // Else, let it cruise along.
        }
    }

    if(current_mode_ == mode::confined)
    {
        if((confinement -= dt) <= sf::Time::Zero)
        {
            behave(requested_mode_);
        }
        else if(utility::length(target_ - getPosition()) < level::tile_size)
        {
            target_ = random_home_corner(home);
        }
    }
    else if(current_mode_ == mode::dead && home.contains(getPosition()))
    {
        healed = true;

        behave(requested_mode_);
    }

    character::update_self(dt, commands);
}

void chaser::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    commands.push(make_command(std::function{[=](layer::characters& characters, sf::Time const&)
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
                auto const closest = std::min_element(heroes_positions.begin(), heroes_positions.end(), [=](auto const& p1, auto const& p2)
                {
                    return utility::length(getPosition() - p1) < utility::length(getPosition() - p2);
                });

                target_ = *closest;
            }
        }
    }}));

    enemy::update_self(dt, commands);
}
using animated_sprite_rects =
    std::array<
        std::vector<sf::IntRect>,
        me::enum_count<enemy::mode>()>;

std::vector<sf::IntRect> goomba_animated_sprite_rects(
    enemy::mode const mode_)
{
    static animated_sprite_rects const rects = []{
        animated_sprite_rects r;

        r[me::enum_integer(enemy::mode::confined)] = std::vector<sf::IntRect>{{1, 28, 16, 16}, {18, 28, 16, 16}};
        r[me::enum_integer(enemy::mode::chase)] = r[me::enum_integer(enemy::mode::confined)];
        r[me::enum_integer(enemy::mode::frightened)] = std::vector<sf::IntRect>{{1, 166, 16, 16}, {18, 166, 16, 16}};
        r[me::enum_integer(enemy::mode::scatter)] = r[me::enum_integer(enemy::mode::confined)];

        return r;
    }();

    return rects[me::enum_integer(mode_)];
}

sf::IntRect goomba_dead_sprite_rect()
{
    return sf::IntRect{39, 28, 16, 16};
}

goomba::goomba(
    sf::FloatRect const& home)
    : chaser{
        goomba_animated_sprite_rects,
        goomba_dead_sprite_rect,
        configuration::values()["enemies"]["goomba"]["scale"].value_or<float>(1.f),
        home,
        *configuration::values()["enemies"]["goomba"]["speed"].value<int>(),
        direction::left
        }
{}

std::string_view goomba::name() const
{
    return "goomba";
}

}
