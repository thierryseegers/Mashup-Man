#pragma once

#include "entity/hero.h"
#include "entity/power_up.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <memory>

namespace entity
{

class brother_power_up_picker;

class brother
    : public hero
{
public:
    enum class size
    {
        small,
        big
    };

    enum class attribute
    {
        plain,
        fiery
    };

    enum class motion
    {
        moving,
        still
    };

    using still_sprite_rect_f = sf::IntRect (*)(
                                    brother::size const,
                                    brother::attribute const);

    using animated_sprite_rects_f = std::vector<sf::IntRect>(*)(
                                        brother::size const,
                                        brother::attribute const);

    using dead_sprite_rect_f = sf::IntRect (*)(
                                    brother::attribute const);

public:
    brother(
        still_sprite_rect_f const still_sprite_rect,
        animated_sprite_rects_f const animated_sprite_rects,
        dead_sprite_rect_f const dead_sprite_rect);

    virtual void hit() override;

    virtual void pick_up(
        power_up::power_up*) override;

    virtual void attack() override;

private:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    virtual void update_sprite() override;

    virtual entity* dead() const override;

    void pick_up(
        power_up::mushroom const* const);

    void pick_up(
        power_up::flower const* const);

    void shoot_fireball(
        layer::projectiles& layer) const;

    still_sprite_rect_f still_sprite_rect;
    animated_sprite_rects_f animated_sprite_rects;
    dead_sprite_rect_f dead_sprite_rect;

    // Attributes.
    size size_;
    attribute attribute_;
    motion motion_;

    // Fireball shooting information.
    sf::Time shoot_cooldown;
    sf::Time fire_countdown;
    bool shooting;

    // Shrinking when hit.
    sf::Time shrinking;

    friend class brother_power_up_picker;
};

struct brother_power_up_picker
    : power_up::power_up_picker
{
    brother_power_up_picker(
        brother* brother_);

    virtual void pick_up(power_up::mushroom const*) override;

    virtual void pick_up(power_up::flower const*) override;

    brother *brother_;
};

template<template<typename> class Hero_N>
class mario
    : public Hero_N<brother>
{
public:
    mario();

    virtual sf::Sprite default_sprite() override;
};

template<template<typename> class Hero_N>
class luigi
    : public Hero_N<brother>
{
public:
    luigi();

    virtual sf::Sprite default_sprite() override;
};

}
