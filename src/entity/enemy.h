#pragma once

#include "command.h"
#include "entity/character.h"
#include "entity/entity.h"
#include "maze.h"
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
        confined,
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
        int const max_speed = 0,
        direction const heading_ = direction::right);

    virtual ~enemy() = default;

    // An enemy is impervious to damage when it is dead and going to the ghost house.
    [[nodiscard]] bool immune() const override;

    void behave(
        mode const m);

    virtual void hit() override;

    virtual std::string_view name() const = 0;

protected:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    virtual void update_sprite() override;

    animated_sprite_rects_f animated_sprite_rects;
    dead_sprite_rect_f dead_sprite_rect;

    mode current_mode_;
    mode requested_mode_;

    maze* maze_;

    sf::Vector2i target_;   // Target coordinates.
    bool healed;            // Whether a hurt ghost has reached home.

    sf::Time confinement;
};

class follower
    : public enemy
{
public:
    using enemy::enemy;

private:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;
};

}
