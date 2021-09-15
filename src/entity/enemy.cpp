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

sf::Vector2f random_corner()
{
    // Pick a random corner area as a target.
    static size_t const x[2] = {5ul, level::width - 5};
    static size_t const y[2] = {5ul, level::width - 5};

    float const c = x[utility::random(1)];
    float const r = y[utility::random(1)];

    return {c * level::tile_size, r * level::tile_size};
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
            animated_sprite_rects(mode::scatter),
            sf::seconds(0.25f),
            sprite::repeat::loop,
            scale_factor
            }
        , max_speed
        , heading_}
    , animated_sprite_rects{animated_sprite_rects}
    , dead_sprite_rect{dead_sprite_rect}
    , mode_{mode::scatter}
    , home{home}
    , healed{true}
    , target{random_corner()}
{}

void enemy::hit()
{
    behave(mode::dead);

    update_sprite();
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
                target = random_corner();
                throttle(1.f);
                break;
        }
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
    if(mode_ == mode::dead && utility::length(getPosition() - target) < level::tile_size)
    {
        healed = true;

        behave(mode::scatter);

        update_sprite();
    }

    character::update_self(dt, commands);
}

using animated_sprite_rects =
    std::array<
        std::vector<sf::IntRect>,
        magic_enum::enum_count<enemy::mode>()>;

std::vector<sf::IntRect> goomba_animated_sprite_rects(
    enemy::mode const mode_)
{
    static animated_sprite_rects const rects = []{
        animated_sprite_rects r;

        r[magic_enum::enum_integer(enemy::mode::chase)] = std::vector<sf::IntRect>{{1, 28, 16, 16}, {18, 28, 16, 16}};
        r[magic_enum::enum_integer(enemy::mode::frightened)] = std::vector<sf::IntRect>{{1, 166, 16, 16}, {18, 166, 16, 16}};
        r[magic_enum::enum_integer(enemy::mode::scatter)] = std::vector<sf::IntRect>{{1, 28, 16, 16}, {18, 28, 16, 16}};

        return r;
    }();

    return rects[magic_enum::enum_integer(mode_)];
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
    std::vector<sf::Vector2f> const& brother_positions,
    std::map<direction, sf::Vector2f> const& choices) const
{
    switch(mode_)
    {
        case mode::scatter:
        case mode::dead:
            return std::min_element(choices.begin(), choices.end(), [=](auto const& c1, auto const& c2)
                {
                    return utility::length(target - c1.second) < utility::length(target - c2.second);
                })->first;
            break;
        case mode::chase:
            {
                assert(brother_positions.size());

                auto const closest = std::min_element(brother_positions.begin(), brother_positions.end(), [=](auto const& p1, auto const& p2)
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
