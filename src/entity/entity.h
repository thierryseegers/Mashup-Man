#pragma once

#include "command.h"
#include "direction.h"
#include "scene.h"
#include "sprite.h"
#include "utility.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>
#include <utility>

namespace entity
{

template<typename T>
class friendly
    : public T
{
public:
    template<typename... Args>
    friendly(
        unsigned int const player_id,
        Args&&... args)
        : player_id_{player_id}
        , T{std::forward<Args>(args)...}
    {}

    unsigned int player_id() const
    {
        return player_id_;
    }

private:
    unsigned int const player_id_;
};

template<typename T>
class hostile
    : public T
{
public:
    using T::T;
};

class entity
    : public scene::node
{
public:
    entity(
        sprite sprite_,
        int const max_speed = 0,
        direction const heading_ = direction::right);

    virtual ~entity() = default;

    virtual sf::FloatRect collision_bounds() const override;

    virtual void hit();

    direction heading() const;

    void head(
        direction const d);

    float speed() const;

    void throttle(
        float const t);

    void nudge(
        float const distance);

    void nudge(
        direction const d,
        float const distance);

protected:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    virtual void draw_self(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    // Sprite update information (e.g. because of a change in throttle).
    virtual void update_sprite();

    // This sprite for this entity.
    sprite sprite_;

    direction heading_;
    int max_speed;
    float throttle_;
};

}
