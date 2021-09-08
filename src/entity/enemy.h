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

    using hostile<character>::hostile;

    virtual ~enemy() = default;

    void behave(
        mode const m);

    virtual direction fork(
        std::vector<sf::Vector2f> const& brother_positions,
        std::map<direction, sf::Vector2f> const& choices) const = 0;

    virtual void hit() override;

    virtual std::string_view name() const = 0;

protected:
    static std::function<void (enemy*)> behavior(
        enemy::mode const before,
        enemy::mode const after);

    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    mode mode_ = mode::scatter;

    sf::Vector2f target;    // Target coordinates.
};

class goomba
    : public enemy
{
public:
    goomba();

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