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

    enemy(
        sprite sprite_,
        int const max_speed,
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

    virtual void draw_self(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    virtual void update_sprite() override;

    mode current_mode_;
    mode requested_mode_;

    maze* maze_;

    sf::Vector2i scatter_corner_;   // The corner this ghost heads towards when scattering.
    sf::Vector2i target_;           // Target coordinates {[0:level::width], [0,level:height]}.
    
    bool healed;            // Whether a hurt ghost has reached home.
    sf::Time confinement;   // Time left in confinement.
};

class follower
    : public enemy
{
public:
    follower(
        sprite sprite_,
        int const max_speed);

private:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;
};

class ahead
    : public enemy
{
public:
    ahead(
        sprite sprite_,
        int const max_speed);

private:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;
};

class axis
    : public enemy
{
public:
    axis(
        sprite sprite_,
        int const max_speed);

private:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    virtual void draw_self(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    sf::Vector2f follower_position_;
    sf::Vector2f follower_to_ahead_;
    sf::Vector2f ahead_to_target_;
};

}
