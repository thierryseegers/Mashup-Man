#include "brothers.h"

#include "command.h"
#include "configuration.h"
#include "fireball.h"
#include "level.h"
#include "sprite.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace entity
{

namespace me = magic_enum;

class dead_brother
    : public entity
{
public:
    dead_brother(
        sprite sprite_)
    : entity{sprite_}
    , velocity{0.f, -5.f}
    {}

    virtual void update_self(
        sf::Time const& dt,
        commands_t&) override
    {
        velocity.y += 9.8f * dt.asSeconds();

        sf::Transformable::move(velocity);
    }

private:
    sf::Vector2f velocity;
};

brother::brother(
    still_sprite_rect_f const still_sprite_rect,
    animated_sprite_rects_f const animated_sprite_rects,
    dead_sprite_rect_f const dead_sprite_rect)
    : hero{
        sprite{
            resources::texture::brothers,
            still_sprite_rect(size::small, attribute::plain),
            configuration::values()["brothers"]["scale"].value_or<float>(1.f)},
        *configuration::values()["brothers"]["speed"].value<int>()}
    , still_sprite_rect(still_sprite_rect)
    , animated_sprite_rects{animated_sprite_rects}
    , dead_sprite_rect{dead_sprite_rect}
    , size_{size::small}
    , attribute_{attribute::plain}
    , motion_{motion::still}
    , shoot_cooldown{sf::seconds(*configuration::values()["brothers"]["fireball"]["cooldown"].value<float>())}
    , fire_countdown{sf::Time::Zero}
    , shooting{false}
    , shrinking{sf::Time::Zero}
{}

void brother::hit()
{
    if(size_ == size::small)
    {
        hero::hit();
    }
    else
    {
        size_ = size::small;
        attribute_ = attribute::plain;
        update_sprite();

        immunity = shrinking = sf::seconds(3);
    }
}

void brother::pick_up(
    power_up::power_up* pu)
{
    brother_power_up_picker picker{this};
    pu->pick_up(&picker);
}

void brother::pick_up(
    power_up::mushroom const* const)
{
    size_ = size::big;

    update_sprite();
}

void brother::pick_up(
    power_up::flower const* const)
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

void brother::attack()
{
    if(attribute_ == attribute::fiery)
    {
        shooting = true;
    }
}

void brother::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if(shrinking > sf::Time::Zero)
    {
        // If we just got hurt, play the sound.
        if(shrinking == sf::seconds(3))
        {
            commands.push(make_command<scene::sound_t>([](scene::sound_t& sound, sf::Time const&)
            {
                sound.play(resources::sound_effect::warp);
            }));
        }

        shrinking = std::max(sf::Time::Zero, shrinking - dt);

        if(shrinking > sf::seconds(1.5f) && (int)(shrinking.asSeconds() * 10) % 2)
        {
            size_ = (size_ == size::small) ? size::big : size::small;
            update_sprite();
        }
        else if(size_ == size::big)
        {
            size_ = size::small;
            update_sprite();
        }
    }

    if(shooting && fire_countdown <= sf::Time::Zero)
    {
        commands.push(make_command<layer::projectiles>([=](layer::projectiles& layer, sf::Time const&)
        {
            shoot_fireball(layer);
        }));

        commands.push(make_command<scene::sound_t>([](scene::sound_t& s, sf::Time const&)
        {
            s.play(resources::sound_effect::fireball);
        }));

        fire_countdown += shoot_cooldown;
        shooting = false;
    }
    else if(fire_countdown > sf::Time::Zero)
    {
        fire_countdown -= dt;
        shooting = false;
    }

    hero::update_self(dt, commands);
}

void brother::update_sprite()
{
    if(throttle_ == 0.f)
    {
        sprite_.still(still_sprite_rect(size_, attribute_));
    }
    else
    {
        sprite_.animate(animated_sprite_rects(size_, attribute_), sf::seconds(0.25f), sprite::repeat::loop);
    }
}

entity* brother::dead() const
{
    return new dead_brother(
        sprite{
            resources::texture::brothers,
            dead_sprite_rect(attribute_),
            configuration::values()["brothers"]["scale"].value_or<float>(1.f)}
    );
}

void brother::shoot_fireball(
    layer::projectiles& layer) const
{
    sf::Vector2f position = getPosition();

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

brother_power_up_picker::brother_power_up_picker(
    brother* brother_)
    : brother_{brother_}
{}
void brother_power_up_picker::pick_up(power_up::mushroom const* m)
{
    brother_->pick_up(m);
}

void brother_power_up_picker::pick_up(power_up::flower const* f)
{
    brother_->pick_up(f);
}

using still_sprite_rects = 
    std::array<
        std::array<
            sf::IntRect,
            me::enum_count<brother::attribute>()>,
        me::enum_count<brother::size>()>;

using animated_sprite_rects = 
    std::array<
        std::array<
            std::vector<sf::IntRect>,
            me::enum_count<brother::attribute>()>,
        me::enum_count<brother::size>()>;

sf::IntRect mario_still_sprite_rect(
    brother::size const size_,
    brother::attribute const attribute_)
{
    static still_sprite_rects const rects = []{
        still_sprite_rects r;

        r[me::enum_integer(brother::size::small)][me::enum_integer(brother::attribute::plain)] = sf::IntRect{1, 9, 16, 16};
        r[me::enum_integer(brother::size::small)][me::enum_integer(brother::attribute::fiery)] = sf::IntRect{1, 140, 16, 16};
        r[me::enum_integer(brother::size::big)][me::enum_integer(brother::attribute::plain)] = sf::IntRect{1, 26, 16, 32};
        r[me::enum_integer(brother::size::big)][me::enum_integer(brother::attribute::fiery)] = sf::IntRect{1, 157, 16, 32};

        return r;
    }();

    return rects[me::enum_integer(size_)][me::enum_integer(attribute_)];
}

std::vector<sf::IntRect> mario_animated_sprite_rects(
    brother::size const size_,
    brother::attribute const attribute_)
{
    static animated_sprite_rects const rects = []{
        animated_sprite_rects r;

        r[me::enum_integer(brother::size::small)][me::enum_integer(brother::attribute::plain)] = std::vector<sf::IntRect>{{43, 9, 16, 16}, {60, 9, 16, 16}, {77, 9, 16, 16}};
        r[me::enum_integer(brother::size::small)][me::enum_integer(brother::attribute::fiery)] = std::vector<sf::IntRect>{{43, 140, 16, 16}, {60, 140, 16, 16}, {77, 140, 16, 16}};

        r[me::enum_integer(brother::size::big)][me::enum_integer(brother::attribute::plain)] = std::vector<sf::IntRect>{{43, 26, 16, 32}, {60, 26, 16, 32}, {77, 26, 16, 32}};
        r[me::enum_integer(brother::size::big)][me::enum_integer(brother::attribute::fiery)] = std::vector<sf::IntRect>{{43, 157, 16, 32}, {60, 157, 16, 32}, {77, 157, 16, 32}};

        return r;
    }();

    return rects[me::enum_integer(size_)][me::enum_integer(attribute_)];
}

sf::IntRect mario_dead_sprite_rect(
    brother::attribute const attribute_)
{
    return attribute_ == brother::attribute::plain ? sf::IntRect{22, 9, 16, 16} : sf::IntRect{1, 140, 16, 16};
}

template<template<typename> class Hero_N>
mario<Hero_N>::mario()
    : Hero_N<brother>{
        mario_still_sprite_rect,
        mario_animated_sprite_rects,
        mario_dead_sprite_rect}
{}

template<template<typename> class Hero_N>
sf::Sprite mario<Hero_N>::default_sprite()
{
    return sf::Sprite{resources::textures().get(resources::texture::brothers), mario_still_sprite_rect(brother::size::small, brother::attribute::plain)};
}

sf::IntRect luigi_still_sprite_rect(
    brother::size const size_,
    brother::attribute const attribute_)
{
    static still_sprite_rects const rects = []{
        still_sprite_rects r;

        r[me::enum_integer(brother::size::small)][me::enum_integer(brother::attribute::plain)] = sf::IntRect{1, 74, 16, 16};
        r[me::enum_integer(brother::size::small)][me::enum_integer(brother::attribute::fiery)] = sf::IntRect{1, 140, 16, 16};
        r[me::enum_integer(brother::size::big)][me::enum_integer(brother::attribute::plain)] = sf::IntRect{1, 91, 16, 32};
        r[me::enum_integer(brother::size::big)][me::enum_integer(brother::attribute::fiery)] = sf::IntRect{1, 157, 16, 32};

        return r;
    }();

    return rects[me::enum_integer(size_)][me::enum_integer(attribute_)];
}

std::vector<sf::IntRect> luigi_animated_sprite_rects(
    brother::size const size_,
    brother::attribute const attribute_)
{
    static animated_sprite_rects const rects = []{
        animated_sprite_rects r;

        r[me::enum_integer(brother::size::small)][me::enum_integer(brother::attribute::plain)] = std::vector<sf::IntRect>{{43, 74, 16, 16}, {60, 74, 16, 16}, {77, 74, 16, 16}};
        r[me::enum_integer(brother::size::small)][me::enum_integer(brother::attribute::fiery)] = std::vector<sf::IntRect>{{43, 140, 16, 16}, {60, 140, 16, 16}, {77, 140, 16, 16}};

        r[me::enum_integer(brother::size::big)][me::enum_integer(brother::attribute::plain)] = std::vector<sf::IntRect>{{43, 91, 16, 32}, {60, 91, 16, 32}, {77, 91, 16, 32}};
        r[me::enum_integer(brother::size::big)][me::enum_integer(brother::attribute::fiery)] = std::vector<sf::IntRect>{{43, 157, 16, 32}, {60, 157, 16, 32}, {77, 157, 16, 32}};

        return r;
    }();

    return rects[me::enum_integer(size_)][me::enum_integer(attribute_)];
}

sf::IntRect luigi_dead_sprite_rect(
    brother::attribute const attribute_)
{
    return attribute_ == brother::attribute::plain ? sf::IntRect{22, 74, 16, 16} : sf::IntRect{22, 9, 16, 16};
}

template<template<typename> class Hero_N>
luigi<Hero_N>::luigi()
    : Hero_N<brother>{
        luigi_still_sprite_rect,
        luigi_animated_sprite_rects,
        luigi_dead_sprite_rect}
{}

template<template<typename> class Hero_N>
sf::Sprite luigi<Hero_N>::default_sprite()
{
    return sf::Sprite{resources::textures().get(resources::texture::brothers), luigi_still_sprite_rect(brother::size::small, brother::attribute::plain)};
}

template class mario<hero_1>;
template class mario<hero_2>;
template class luigi<hero_1>;
template class luigi<hero_2>;

}