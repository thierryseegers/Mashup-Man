#pragma once

#include "command.h"
#include "direction.h"
#include "entity/character.h"
#include "entity/entity.h"
#include "layer.h"
#include "scene.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

namespace entity
{

class brother
    : public friendly<character>
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

    enum class liveness
    {
        alive,
        dead
    };

private:
    using still_sprite_rect_f = sf::IntRect (*)(
                                    brother::size const,
                                    brother::attribute const,
                                    brother::liveness const);

    using animated_sprite_rects_f = std::vector<sf::IntRect>(*)(
                                        brother::size const,
                                        brother::attribute const);

public:
    brother(
        direction const facing_,
        still_sprite_rect_f still_sprite_rect,
        animated_sprite_rects_f animated_sprite_rects);

    virtual ~brother() = default;

    [[nodiscard]] direction steering() const;

    void steer(
        direction const d);

    void fire();

    void consume_mushroom();

    void consume_flower();

    void hit();

protected:
    virtual void update_sprite() override;

    void shoot_fireball(
        layer::projectiles& layer) const;

    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    still_sprite_rect_f still_sprite_rect;
    animated_sprite_rects_f animated_sprite_rects;

    direction steering_;

    size size_;
    attribute attribute_;
    motion motion_;
    liveness liveness_;

    sf::Time fire_cooldown;
    sf::Time fire_countdown;
    bool firing;
};

class mario
    : public brother
{
public:
    mario();
};

class luigi
    : public brother
{
public:
    luigi();
};

}
