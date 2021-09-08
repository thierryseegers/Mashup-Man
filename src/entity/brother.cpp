#include "entity/brother.h"

#include "command.h"
#include "configuration.h"
#include "entity/character.h"
#include "entity/entity.h"
#include "entity/fireball.h"
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

class dead
    : public entity
{
public:
    using entity::entity;

    // virtual void update_self(
    //     sf::Time const& dt,
    //     commands_t& commands) override
    // {

    // }
};

brother::brother(
    direction const facing_,
    still_sprite_rect_f const still_sprite_rect,
    animated_sprite_rects_f const animated_sprite_rects,
    dead_sprite_rect_f const dead_sprite_rect)
    : friendly<character>{
        sprite{
            resources::texture::brothers,
            still_sprite_rect(size::small, attribute::plain),
            configuration::values()["brothers"]["scale"].value_or<float>(1.f)},
        *configuration::values()["brothers"]["speed"].value<int>(),
        facing_}
    , still_sprite_rect(still_sprite_rect)
    , animated_sprite_rects{animated_sprite_rects}
    , dead_sprite_rect{dead_sprite_rect}
    , size_{size::small}
    , attribute_{attribute::plain}
    , motion_{motion::still}
    , liveness_{liveness::alive}
    , fire_cooldown{sf::seconds(*configuration::values()["brothers"]["fireball"]["cooldown"].value<float>())}
    , fire_countdown{sf::Time::Zero}
    , firing{false}
{}

direction brother::steering() const
{
    return steering_;
}

void brother::steer(
    direction const d)
{
    steering_ = d;
}

void brother::fire()
{
    if(attribute_ == attribute::fiery)
    {
        firing = true;
    }
}

void brother::consume_mushroom()
{
    size_ = size::big;

    update_sprite();
}

void brother::consume_flower()
{
    if(size_ == size::small)
    {
        size_ = size::big;
    }
    else
    {
        attribute_ = attribute::fiery;
    }

    update_sprite();
}

void brother::hit()
{
    if(size_ == size::small)
    {
        remove = true;
    }
    else
    {
        size_ = size::small;
        attribute_ = attribute::plain;

        update_sprite();
    }
}

void brother::update_sprite()
{
    if(throttle_ == 0.f)
    {
        sprite_.still(still_sprite_rect(size_, attribute_));
    }
    else
    {
        sprite_.animate(animated_sprite_rects(size_, attribute_), sf::seconds(0.5f), sprite::repeat::loop);
    }
}

void brother::shoot_fireball(
    layer::projectiles& layer) const
{
    auto position = getPosition();

    switch(heading_)
    {
        case direction::up:
            position.y -= level::tile_size;
            break;
        case direction::down:
            position.y += level::tile_size;
            break;
        case direction::left:
            position.x -= level::tile_size;
            break;
        case direction::right:
            position.x += level::tile_size;
            break;
        default:
            break;
    }

    add_projectile<fireball>(layer, position, heading_);
}

void brother::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if(remove)
    {
        commands.push(make_command<layer::animations>([rect = dead_sprite_rect(attribute_), position = getPosition()](layer::animations& layer, sf::Time const&)
        {
            layer.attach<dead>(sprite{resources::texture::brothers, rect})->setPosition(position);
        }));
    }
    else
    {
        if(firing && fire_countdown <= sf::Time::Zero)
        {
            commands.push(make_command<layer::projectiles>([=](layer::projectiles& layer, sf::Time const&)
            {
                shoot_fireball(layer);
            }));

            commands.push(make_command<scene::sound_t>([](scene::sound_t& s, sf::Time const&)
            {
                s.play(resources::sound_effect::fireball);
            }));

            fire_countdown += fire_cooldown;
            firing = false;
        }
        else if(fire_countdown > sf::Time::Zero)
        {
            fire_countdown -= dt;
            firing = false;
        }

        character::update_self(dt, commands);
    }
}

using still_sprite_rects = 
    std::array<
        std::array<
            sf::IntRect,
            magic_enum::enum_count<brother::attribute>()>,
        magic_enum::enum_count<brother::size>()>;

using animated_sprite_rects = 
    std::array<
        std::array<
            std::vector<sf::IntRect>,
            magic_enum::enum_count<brother::attribute>()>,
        magic_enum::enum_count<brother::size>()>;

sf::IntRect mario_still_sprite_rect(
    brother::size const size_,
    brother::attribute const attribute_)
{
    static still_sprite_rects const rects = []{
        still_sprite_rects r;

        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::plain)] = sf::IntRect{1, 9, 16, 16};
        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::fiery)] = sf::IntRect{1, 140, 16, 16};
        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::plain)] = sf::IntRect{1, 26, 16, 32};
        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::fiery)] = sf::IntRect{1, 157, 16, 32};

        return r;
    }();

    return rects[magic_enum::enum_integer(size_)][magic_enum::enum_integer(attribute_)];
}

std::vector<sf::IntRect> mario_animated_sprite_rects(
    brother::size const size_,
    brother::attribute const attribute_)
{
    static animated_sprite_rects const rects = []{
        animated_sprite_rects r;

        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::plain)] = std::vector<sf::IntRect>{{43, 9, 16, 16}, {60, 9, 16, 16}, {77, 9, 16, 16}};
        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::fiery)] = std::vector<sf::IntRect>{{43, 140, 16, 16}, {60, 140, 16, 16}, {77, 140, 16, 16}};

        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::plain)] = std::vector<sf::IntRect>{{43, 26, 16, 32}, {60, 26, 16, 32}, {77, 26, 16, 32}};
        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::fiery)] = std::vector<sf::IntRect>{{43, 157, 16, 32}, {60, 157, 16, 32}, {77, 157, 16, 32}};

        return r;
    }();

    return rects[magic_enum::enum_integer(size_)][magic_enum::enum_integer(attribute_)];
}

sf::IntRect mario_dead_sprite_rect(
    brother::attribute const attribute_)
{
    return attribute_ == brother::attribute::plain ? sf::IntRect{22, 9, 16, 16} : sf::IntRect{1, 140, 16, 16};
}

mario::mario()
    : brother{
        direction::right,
        mario_still_sprite_rect,
        mario_animated_sprite_rects,
        mario_dead_sprite_rect}
{}

sf::IntRect luigi_still_sprite_rect(
    brother::size const size_,
    brother::attribute const attribute_)
{
    static still_sprite_rects const rects = []{
        still_sprite_rects r;

        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::plain)] = sf::IntRect{1, 74, 16, 16};
        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::fiery)] = sf::IntRect{1, 140, 16, 16};
        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::plain)] = sf::IntRect{1, 91, 16, 32};
        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::fiery)] = sf::IntRect{1, 157, 16, 32};

        return r;
    }();

    return rects[magic_enum::enum_integer(size_)][magic_enum::enum_integer(attribute_)];
}

std::vector<sf::IntRect> luigi_animated_sprite_rects(
    brother::size const size_,
    brother::attribute const attribute_)
{
    static animated_sprite_rects const rects = []{
        animated_sprite_rects r;

        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::plain)] = std::vector<sf::IntRect>{{43, 74, 16, 16}, {60, 74, 16, 16}, {77, 74, 16, 16}};
        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::fiery)] = std::vector<sf::IntRect>{{43, 140, 16, 16}, {60, 140, 16, 16}, {77, 140, 16, 16}};

        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::plain)] = std::vector<sf::IntRect>{{43, 91, 16, 32}, {60, 91, 16, 32}, {77, 91, 16, 32}};
        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::fiery)] = std::vector<sf::IntRect>{{43, 157, 16, 32}, {60, 157, 16, 32}, {77, 157, 16, 32}};

        return r;
    }();

    return rects[magic_enum::enum_integer(size_)][magic_enum::enum_integer(attribute_)];
}

sf::IntRect luigi_dead_sprite_rect(
    brother::attribute const attribute_)
{
    return attribute_ == brother::attribute::plain ? sf::IntRect{22, 74, 16, 16} : sf::IntRect{22, 9, 16, 16};
}

luigi::luigi()
    : brother{
        direction::left,
        luigi_still_sprite_rect,
        luigi_animated_sprite_rects,
        luigi_dead_sprite_rect}
{}

}
