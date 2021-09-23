#pragma once

#include "command.h"
#include "direction.h"
#include "entity/character.h"
#include "layer.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

#if defined(WIN32)
    #undef small
#endif

namespace entity
{

class hero
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

private:
    using still_sprite_rect_f = sf::IntRect (*)(
                                    hero::size const,
                                    hero::attribute const);

    using animated_sprite_rects_f = std::vector<sf::IntRect>(*)(
                                        hero::size const,
                                        hero::attribute const);

    using dead_sprite_rect_f = sf::IntRect (*)(
                                    hero::attribute const);

public:
    hero(
        direction const facing_,
        still_sprite_rect_f const still_sprite_rect,
        animated_sprite_rects_f const animated_sprite_rects,
        dead_sprite_rect_f const dead_sprite_rect);

    virtual ~hero() = default;

    virtual void hit() override;

    [[nodiscard]] direction steering() const;

    [[nodiscard]] bool untouchable() const;

    void steer(
        direction const d);

    void fire();

    void consume_mushroom();

    void consume_flower();

protected:
    virtual void update_sprite() override;

    void shoot_fireball(
        layer::projectiles& layer) const;

    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    still_sprite_rect_f still_sprite_rect;
    animated_sprite_rects_f animated_sprite_rects;
    dead_sprite_rect_f dead_sprite_rect;

    direction steering_;

    size size_;
    attribute attribute_;
    motion motion_;

    sf::Time fire_cooldown;
    sf::Time fire_countdown;
    bool firing;

    sf::Time shrinking;
    sf::Time untouchable_;
};

class mario
    : public hero
{
public:
    mario();

    static sf::Sprite default_sprite();
};

class luigi
    : public hero
{
public:
    luigi();

    static sf::Sprite default_sprite();
};

}
