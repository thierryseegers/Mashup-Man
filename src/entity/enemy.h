#pragma once

#include "command.h"
#include "entity/character.h"
#include "entity/entity.h"
#include "resources.h"
#include "scene.h"
#include "sprite.h"
#include "utility.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

#include <map>
#include <string_view>
#include <vector>

namespace entity
{

class enemy
    : public hostile<character>
{
public:
    enum class mode
    {
        scatter,
        chase,
        frightened,
        dead
    };

private:
    using animated_sprite_rects_f = std::vector<sf::IntRect>(*)(
                                        mode const mode_);

    using dead_sprite_rect_f = sf::IntRect (*)();

public:
    enemy(
        animated_sprite_rects_f const animated_sprite_rects,
        dead_sprite_rect_f const dead_sprite_rect,
        float const scale_factor,
        sf::IntRect const& home,
        int const max_speed = 0,
        direction const heading_ = direction::right);

    virtual ~enemy() = default;

    [[nodiscard]] mode behavior() const;

    void behave(
        mode const m);

    virtual direction fork(
        std::vector<sf::Vector2f> const& brother_positions,
        std::map<direction, sf::Vector2f> const& choices) const = 0;

    virtual void hit() override;

    virtual std::string_view name() const = 0;

protected:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    virtual void update_sprite() override;

    animated_sprite_rects_f animated_sprite_rects;
    dead_sprite_rect_f dead_sprite_rect;

    mode mode_;

    sf::IntRect home;       // The ghost home.
    bool healed;            // Whether a hurt ghost has reached home.

    sf::Vector2f target;    // Target coordinates.
};

class goomba
    : public enemy
{
public:
    goomba(
        sf::IntRect const& home);

    virtual direction fork(
        std::vector<sf::Vector2f> const& brother_positions,
        std::map<direction, sf::Vector2f> const& choices) const override;

    virtual std::string_view name() const override;
};

// class raptor : public enemy
// {
// public:
//     raptor();

// protected:
//     virtual void attack(
//         scene::projectiles& layer) const override;
// };

}