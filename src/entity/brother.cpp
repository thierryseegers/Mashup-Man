#include "entity/brother.h"

#include "command.h"
#include "configuration.h"
#include "entity/character.h"
// #include "entity/bullet.h"
#include "entity/entity.h"
// #include "entity/missile.h"
// #include "entity/projectile.h"
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
    sprite const& sprite_)
    : friendly<character>{sprite_}
    , size_{size::small}
    , attribute_{attribute::plain}
    , motion_{motion::still}
    , liveness_{liveness::alive}
    // , fire_rate{1}
    // , fire_countdown{sf::Time::Zero}
    // , firing{false}
{}

void brother::set_direction(
    sf::Vector2f const& direction)
{
    bool sprite_update = false;

    if(direction.x == 0.f && direction.y == 0.f &&
       (velocity.x != 0.f || velocity.y != 0.f))
    {
        sprite_update = true;
    }
    else if((direction.x != 0.f || direction.y != 0.f) &&
            velocity.x == 0.f && velocity.y == 0.f)
    {
        sprite_update = true;
    }

    if(velocity.x >= 0 && direction.x < 0)
    {
        sprite_.flip();
        sprite_.set_rotation(0);
    }
    else if(velocity.x <= 0 && direction.x > 0)
    {
        sprite_.unflip();
        sprite_.set_rotation(0);
    }
    else if(velocity.y >= 0 && direction.y < 0)
    {
        sprite_.unflip();
        sprite_.set_rotation(270.f);
    }
    else if(velocity.y <= 0 && direction.y >0)
    {
        sprite_.unflip();
        sprite_.set_rotation(90.f);
    }

    velocity = direction;

    if(sprite_update)
    {
        update_sprite();
    }
}

void brother::fire()
{
    // firing = true;
}

void brother::consume_mushroom()
{
    size_ = size::big;

    update_sprite();
}

void brother::consume_flower()
{
    attribute_ = attribute::fiery;

    update_sprite();
}

void brother::hit_enemy()
{
    switch(size_)
    {
        case size::small:
            // I'm dead.
            break;
        case size::big:
            size_ = size::small;
            attribute_ = attribute::plain;
            break;
    }
}

void brother::hit_fireball()
{}

void brother::update_self(
        sf::Time const& dt,
        commands_t& commands)
{
    // {
    //     auto texture_rect = default_texture_rect;
    //     if(velocity.x < 0.f)
    //     {
    //         texture_rect.left += default_texture_rect.width;
    //     }
    //     else if(velocity.x > 0.f)
    //     {
    //         texture_rect.left += 2 * default_texture_rect.width;
    //     }

    //     sprite.setTextureRect(texture_rect);
    // }

    // if(firing && fire_countdown <= sf::Time::Zero)
    // {
    //     commands.push(make_command<scene::projectiles>([=](scene::projectiles& layer, sf::Time const&)
    //     {
    //         shoot_bullet(layer);
    //     }));

    //     play_local_sound(commands, resources::sound_effect::allied_gunfire);

    //     fire_countdown += sf::seconds(1.f / (fire_rate + 1));
    //     firing = false;
    // }
    // else if(fire_countdown > sf::Time::Zero)
    // {
    //     fire_countdown -= dt;
    //     firing = false;
    // }

    character::update_self(dt, commands);
}

void brother::update_sprite()
{
    if(velocity.x == 0.f && velocity.y == 0.f)
    {
        sprite_.still(still_sprite_rect_(size_, attribute_, liveness_));
    }
    else
    {
        sprite_.animate(animated_sprite_rects_(size_, attribute_), sf::seconds(0.5f), sprite::repeat::loop);
    }
}
// void brother::shoot_bullet(
//     scene::projectiles& layer) const
// {
//     switch(bullet_spread)
//     {
//     case 1:
//         add_projectile<bullet<friendly>>(layer, {0.f, 0.5f}, projectile::upward);
//         break;
//     case 2:
//         add_projectile<bullet<friendly>>(layer, {-0.33f, 0.33f}, projectile::upward);
//         add_projectile<bullet<friendly>>(layer, {0.33f, 0.33f}, projectile::upward);
//         break;
//     case 3:
//         add_projectile<bullet<friendly>>(layer, {0.f, 0.5f}, projectile::upward);
//         add_projectile<bullet<friendly>>(layer, {-0.33f, 0.33f}, projectile::upward);
//         add_projectile<bullet<friendly>>(layer, {0.33f, 0.33f}, projectile::upward);
//         break;
//     }
// }


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
        sprite{
            resources::texture::brothers,
            mario_still_sprite_rect(size::small, attribute::plain, liveness::alive),
            configuration::values()["brothers"]["scale"].value_or<float>(1.f)}}
{
    still_sprite_rect_ = mario_still_sprite_rect;
    animated_sprite_rects_ = mario_animated_sprite_rects;
}

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
        sprite{
            resources::texture::brothers,
            luigi_still_sprite_rect(size::small, attribute::plain, liveness::alive),
            configuration::values()["brothers"]["scale"].value_or<float>(1.f)}}
{
    still_sprite_rect_ = luigi_still_sprite_rect;
    animated_sprite_rects_ = luigi_animated_sprite_rects;

}

}
