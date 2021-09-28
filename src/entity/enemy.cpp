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
    return current_mode_ == mode::dead;
}

void enemy::behave(
    mode const requested_mode)
{
    if(current_mode_ == requested_mode ||
       (current_mode_ == mode::dead && !healed) ||
       (current_mode_ == mode::confined && confinement >= sf::Time::Zero))
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
        case mode::step_out:
            target = {level::width * level::tile_size / 2, 0};  //Just need to get out of the house.
            throttle(1.f);
            break;
        case mode::chase:
            throttle(1.f);
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
        behave(mode::step_out);
    }
    // else if(current_mode_ == mode::dead && contains(world_transform().transformRect(home), collision_bounds()))
    // else if(current_mode_ == mode::dead && contains(home, getInverseTransform().transformRect(sprite_.global_bounds())))
    // else if(current_mode_ == mode::dead && contains(home, sprite_.sprite_.getLocalBounds()))
    else if(current_mode_ == mode::dead && home.contains(getPosition()))
    {
        healed = true;

        behave(mode::step_out);
    }
    else if(current_mode_ == mode::step_out && !home.contains(getPosition()))
    {
        behave(requested_mode_);
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
    sf::FloatRect const& home)
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
    switch(current_mode_)
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
