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

    using hostile<character>::hostile;

    virtual ~enemy() = default;

    virtual direction fork(
        std::vector<sf::Vector2f> const& brother_positions,
        std::map<direction, sf::Vector2f> const& choices) const = 0;

    virtual void hit() override;

protected:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    mode mode_ = mode::chase;

    // virtual void attack(
    //     scene::projectiles& layer) const = 0;

    // float const attack_rate;
    // sf::Time attack_countdown;
};

class goomba
    : public enemy
{
public:
    goomba();

    virtual direction fork(
        std::vector<sf::Vector2f> const& brother_positions,
        std::map<direction, sf::Vector2f> const& choices) const override;
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