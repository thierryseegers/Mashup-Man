#include "entity/super_mario/hero.h"

#include "command.h"
#include "configuration.h"
#include "entity/super_mario/picker.h"
#include "entity/super_mario/power_up.h"
#include "entity/super_mario/projectile.h"
#include "level.h"
#include "sprite.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace entity::super_mario
{

namespace me = magic_enum;

class dead_brother
    : public entity
{
public:
    dead_brother(
        sprite sprite_)
    : entity{sprite_}
    , velocity{0.f, 32 * -5.f}
    {}

    virtual void update_self(
        sf::Time const& dt,
        commands_t&) override
    {
        velocity.y += 32 * 9.8f * dt.asSeconds();

        sf::Transformable::move(velocity * dt.asSeconds());
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
            still_sprite_rect(size::small, attribute::plain)},
        configuration::value<int>()["brothers"]["speed"]}
    , still_sprite_rect(still_sprite_rect)
    , animated_sprite_rects{animated_sprite_rects}
    , dead_sprite_rect{dead_sprite_rect}
    , size_{size::small}
    , attribute_{attribute::plain}
    , motion_{motion::still}
    , shoot_cooldown{sf::seconds(configuration::value<float>()["brothers"]["fireball"]["cooldown"])}
    , fire_countdown{sf::Time::Zero}
    , shooting{false}
    , shrinking{sf::Time::Zero}
{
    auto const scale = configuration::value_or<float>(1.f)["brothers"]["scale"];
    setScale(scale, scale);
}

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
    power_up* pu)
{
    super_mario::brother_picker picker{this};
    pu->picked_up(&picker);
}

void brother::pick_up(
    super_mario::coin const* const)
{}

void brother::pick_up(
    super_mario::mushroom const* const)
{
    size_ = size::big;

    update_sprite();
}

void brother::pick_up(
    super_mario::flower const* const)
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
            commands.push(make_command(+[](scene::sound_player& sound, sf::Time const&)
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
        commands.push(make_command(std::function{[this](layer::projectiles& layer, sf::Time const&)
        {
            shoot_fireball(layer);
        }}));

        commands.push(make_command(+[](scene::sound_player& sound, sf::Time const&)
        {
            sound.play(resources::sound_effect::fireball);
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

    character::update_sprite();
}

entity* brother::tombstone() const
{
    auto *db = new dead_brother(sprite{resources::texture::brothers, dead_sprite_rect(attribute_)});
    auto const scale = configuration::value_or<float>(1.f)["brothers"]["scale"];
    db->setScale(scale, scale);

    return db;
}

void brother::shoot_fireball(
    layer::projectiles& layer) const
{
    sf::Vector2f const position = getPosition() + to_vector2f(heading_) * static_cast<float>(level::tile_size);

    add_projectile<super_mario::fireball>(layer, position, heading_);
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

mario::mario()
    : brother{
        mario_still_sprite_rect,
        mario_animated_sprite_rects,
        mario_dead_sprite_rect}
{}

sprite mario::default_still()
{
    return {resources::texture::brothers, mario_still_sprite_rect(brother::size::small, brother::attribute::plain)};
}

sprite mario::default_animated()
{
    return {resources::texture::brothers, mario_animated_sprite_rects(brother::size::small, brother::attribute::plain), sf::seconds(0.25f), sprite::repeat::loop};
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

luigi::luigi()
    : brother{
        luigi_still_sprite_rect,
        luigi_animated_sprite_rects,
        luigi_dead_sprite_rect}
{}

sprite luigi::default_still()
{
    return {resources::texture::brothers, luigi_still_sprite_rect(brother::size::small, brother::attribute::plain)};
}

sprite luigi::default_animated()
{
    return {resources::texture::brothers, luigi_animated_sprite_rects(brother::size::small, brother::attribute::plain), sf::seconds(0.25f), sprite::repeat::loop};
}

}
