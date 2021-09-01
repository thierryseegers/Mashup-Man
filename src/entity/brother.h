#pragma once

#include "command.h"
#include "entity/character.h"
#include "entity/entity.h"
#include "scene.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

#include <string>

namespace entity
{

class brother : public friendly<character>
{
public:
    brother(
        std::string const& name);

    virtual ~brother() = default;

    void fire();

    void consume_mushroom();

    void consume_flower();

    void hit_enemy();

    void hit_fireball();

    // void launch_missile();

    // void increase_fire_rate();

    // void increase_bullet_spread();

    // void collect_missile(
    //     int const amount);

private:
    enum class size
    {
        small,
        big
    } size;

    enum class attribute
    {
        plain,
        fiery
    } attribute;

    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    // void shoot_bullet(
    //     scene::projectiles& layer) const;

    // void shoot_missile(
    //     scene::projectiles& layer) const;

    // sf::IntRect const default_texture_rect; // Texture rectangle for leader flying straight.

    // int bullet_spread;
    // int fire_rate;
    // sf::Time fire_countdown;
    // bool firing;

    // int missile_ammo;
    // bool missile_guidance;
    // bool launching_missile;
};

}
