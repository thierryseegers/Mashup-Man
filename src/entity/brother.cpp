#include "entity/brother.h"

#include "command.h"
#include "configuration.h"
#include "entity/character.h"
#include "entity/entity.h"
#include "entity/fireball.h"
#include "layer.h"
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

brother::brother(
    direction const facing_,
    still_sprite_rect_f still_sprite_rect,
    animated_sprite_rects_f animated_sprite_rects)
    : friendly<character>{
        sprite{
            resources::texture::brothers,
            still_sprite_rect(size::small, attribute::plain, liveness::alive),
            configuration::values()["brothers"]["scale"].value_or<float>(1.f)},
        *configuration::values()["brothers"]["speed"].value<int>(),
        facing_}
    , still_sprite_rect(still_sprite_rect)
    , animated_sprite_rects{animated_sprite_rects}
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
        liveness_ = liveness::dead;
    }
    else
    {
        size_ = size::small;
        attribute_ = attribute::plain;
    }

    update_sprite();
}

void brother::update_sprite()
{
    if(throttle_ == 0.f)
    {
        sprite_.still(still_sprite_rect(size_, attribute_, liveness_));
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
            position.y -= 20;
            break;
        case direction::down:
            position.y += 20;
            break;
        case direction::left:
            position.x -= 20;
            break;
        case direction::right:
            position.x += 20;
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

using still_sprite_rects = 
    std::array<
        std::array<
            std::array<
                sf::IntRect,
                magic_enum::enum_count<brother::liveness>()>,
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
    brother::attribute const attribute_,
    brother::liveness const liveness_)
{
    static still_sprite_rects const rects = []{
        still_sprite_rects r;

        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::plain)][magic_enum::enum_integer(brother::liveness::alive)] = sf::IntRect{1, 9, 16, 16};
        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::plain)][magic_enum::enum_integer(brother::liveness::dead)] = sf::IntRect{22, 9, 16, 16};
        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::fiery)][magic_enum::enum_integer(brother::liveness::alive)] = sf::IntRect{1, 140, 16, 16};
        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::fiery)][magic_enum::enum_integer(brother::liveness::dead)] = sf::IntRect{22, 140, 16, 16};

        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::plain)][magic_enum::enum_integer(brother::liveness::alive)] = sf::IntRect{1, 26, 16, 32};
        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::plain)][magic_enum::enum_integer(brother::liveness::dead)] = r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::plain)][magic_enum::enum_integer(brother::liveness::dead)];
        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::fiery)][magic_enum::enum_integer(brother::liveness::alive)] = sf::IntRect{1, 157, 16, 32};
        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::fiery)][magic_enum::enum_integer(brother::liveness::dead)] = r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::fiery)][magic_enum::enum_integer(brother::liveness::dead)];

        return r;
    }();

    return rects[magic_enum::enum_integer(size_)][magic_enum::enum_integer(attribute_)][magic_enum::enum_integer(liveness_)];
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

mario::mario()
    : brother{
        direction::right,
        mario_still_sprite_rect,
        mario_animated_sprite_rects}
{}

sf::IntRect luigi_still_sprite_rect(
    brother::size const size_,
    brother::attribute const attribute_,
    brother::liveness const liveness_)
{
    static still_sprite_rects const rects = []{
        still_sprite_rects r;

        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::plain)][magic_enum::enum_integer(brother::liveness::alive)] = sf::IntRect{1, 74, 16, 16};
        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::plain)][magic_enum::enum_integer(brother::liveness::dead)] = sf::IntRect{22, 74, 16, 16};
        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::fiery)][magic_enum::enum_integer(brother::liveness::alive)] = sf::IntRect{1, 140, 16, 16};
        r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::fiery)][magic_enum::enum_integer(brother::liveness::dead)] = sf::IntRect{22, 9, 16, 16};

        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::plain)][magic_enum::enum_integer(brother::liveness::alive)] = sf::IntRect{1, 91, 16, 32};
        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::plain)][magic_enum::enum_integer(brother::liveness::dead)] = r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::plain)][magic_enum::enum_integer(brother::liveness::dead)];
        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::fiery)][magic_enum::enum_integer(brother::liveness::alive)] = sf::IntRect{1, 157, 16, 32};
        r[magic_enum::enum_integer(brother::size::big)][magic_enum::enum_integer(brother::attribute::fiery)][magic_enum::enum_integer(brother::liveness::dead)] = r[magic_enum::enum_integer(brother::size::small)][magic_enum::enum_integer(brother::attribute::fiery)][magic_enum::enum_integer(brother::liveness::dead)];

        return r;
    }();

    return rects[magic_enum::enum_integer(size_)][magic_enum::enum_integer(attribute_)][magic_enum::enum_integer(liveness_)];
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

luigi::luigi()
    : brother{
        direction::left,
        luigi_still_sprite_rect,
        luigi_animated_sprite_rects}
{}

}
