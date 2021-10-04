#include "entity/enemy.h"

#include "configuration.h"
#include "direction.h"
#include "entity/character.h"
#include "level.h"
#include "resources.h"
#include "scene.h"
#include "sprite.h"
#include "tomlpp.h"
#include "utility.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>

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
    static size_t const y[2] = {2ul, level::height - 2 - 2};

    float const c = x[utility::random(1)];
    float const r = y[utility::random(1)];

    return {c * level::tile_size, r * level::tile_size};
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
}

void enemy::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if(current_mode_ == mode::confined && ((confinement -= dt) <= sf::Time::Zero))
    {
        behave(requested_mode_);
    }
    else if(current_mode_ == mode::dead && home.contains(getPosition()))
    {
        healed = true;

        behave(requested_mode_);
    }

    character::update_self(dt, commands);
}


sf::Vector2f chaser::target(
    std::vector<std::pair<sf::Vector2f, direction>> const& heroes,
    sf::Vector2f const& chaser)
{
    switch(current_mode_)
    {
        case mode::confined:
            if(utility::length(target_ - getPosition()) < level::tile_size)
            {
                target_ = random_home_corner(home);
            }
            break;
        case mode::chase:
            {
                assert(heroes.size());

                auto const closest = std::min_element(heroes.begin(), heroes.end(), [=](auto const& p1, auto const& p2)
                {
                    return utility::length(getPosition() - p1.first) < utility::length(getPosition() - p2.first);
                });

                target_ = closest->first;
            }
            break;
        case mode::frightened:
            break;
        default:
            break;
    }

    return target_;
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
    static std::string const name_{"goomba"};

    return name_;
}

}
