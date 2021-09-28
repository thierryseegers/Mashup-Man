#include "entity/enemy.h"

#include "configuration.h"
#include "direction.h"
#include "entity/entity.h"
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
    static size_t const x[2] = {5ul, level::width - 5};
    static size_t const y[2] = {5ul, level::width - 5};

    float const c = x[utility::random(1)];
    float const r = y[utility::random(1)];

    return {c * level::tile_size, r * level::tile_size};
}

sf::Vector2f random_home_corner(
    sf::IntRect const& home)
{
    return {(float)home.left + utility::random(1) * (home.width - level::tile_size) + level::half_tile_size,
            (float)home.top + utility::random(1) * (home.height - level::tile_size) + level::half_tile_size};
}

enemy::enemy(
    animated_sprite_rects_f const animated_sprite_rects,
    dead_sprite_rect_f const dead_sprite_rect,
    float const scale_factor,
    sf::IntRect const& home,
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
    , mode_{mode::confined}
    , home{home}
    , healed{true}
    , target{random_home_corner(home)}
    , confinement{sf::seconds(10)}
{}

void enemy::hit()
{
    behave(mode::dead);

    update_sprite();
}

bool enemy::immune() const
{
    return mode_ == mode::dead;
}

enemy::mode enemy::behavior() const
{
    return mode_;
}

void enemy::behave(
    mode const m)
{
    if(mode_ != m && healed)
    {
        spdlog::info("{} changing mode from {} to {}.", name(), magic_enum::enum_name(mode_), magic_enum::enum_name(m));

        mode_ = m;

        switch(mode_)
        {
            case mode::confined:
                assert(!"Do not manually change behavior to 'confined'. This mode is set form the start and never reoccurs.");
                break;
            case mode::chase:
                break;
            case mode::dead:
                healed = false;
                target = {home.left + home.width / 2.f, home.top + home.height / 2.f};
                throttle(2.f);
                break;
            case mode::frightened:
                break;
            case mode::scatter:
                target = random_level_corner();
                throttle(1.f);
                break;
        }

        update_sprite();
    }
}

void enemy::update_sprite()
{
    if(mode_ == mode::dead)
    {
        sprite_.still(dead_sprite_rect());
    }
    else
    {
        sprite_.animate(animated_sprite_rects(mode_), sf::seconds(0.25f), sprite::repeat::loop);
    }
}

void enemy::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if(mode_ == mode::confined && ((confinement -= dt) <= sf::Time::Zero))
    {
        behave(mode::scatter);
    }
    else if(mode_ == mode::dead && utility::length(getPosition() - target) < level::tile_size)
    {
        healed = true;

        behave(mode::scatter);
    }

    character::update_self(dt, commands);
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
        r[me::enum_integer(enemy::mode::step_out)] = r[me::enum_integer(enemy::mode::confined)];

        return r;
    }();

    return rects[me::enum_integer(mode_)];
}

sf::IntRect goomba_dead_sprite_rect()
{
    return sf::IntRect{39, 28, 16, 16};
}

goomba::goomba(
    sf::IntRect const& home)
    : enemy{
        goomba_animated_sprite_rects,
        goomba_dead_sprite_rect,
        configuration::values()["enemies"]["goomba"]["scale"].value_or<float>(1.f),
        home,
        *configuration::values()["enemies"]["goomba"]["speed"].value<int>(),
        direction::left
        }
{}

direction goomba::fork(
    std::vector<sf::Vector2f> const& heroes_positions,
    std::map<direction, sf::Vector2f> const& choices)
{
    switch(mode_)
    {
        case mode::confined:
            if(utility::length(target - getPosition()) < level::tile_size)
            {
                target = random_home_corner(home);
            }
            [[fallthrough]];
        case mode::scatter:
        case mode::dead:
            return std::min_element(choices.begin(), choices.end(), [=](auto const& c1, auto const& c2)
                {
                    return utility::length(target - c1.second) < utility::length(target - c2.second);
                })->first;
            break;
        case mode::chase:
            {
                assert(heroes_positions.size());

                auto const closest = std::min_element(heroes_positions.begin(), heroes_positions.end(), [=](auto const& p1, auto const& p2)
                {
                    return utility::length(getPosition() - p1) < utility::length(getPosition() - p2);
                });

                return std::min_element(choices.begin(), choices.end(), [=](auto const& c1, auto const& c2)
                {
                    return utility::length(*closest - c1.second) < utility::length(*closest - c2.second);
                })->first;
            }
            break;
        case mode::frightened:
            break;
        default:
            break;
    }

    return choices.begin()->first;
}

std::string_view goomba::name() const
{
    static std::string const name_{"goomba"};

    return name_;
}

}
