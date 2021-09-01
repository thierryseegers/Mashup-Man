#pragma once

#include "command.h"
#include "entity/entity.h"
// #include "projectile.h"
#include "resources.h"
#include "scene.h"

#include <SFML/System/Vector2.hpp>

#include <utility>

namespace entity
{

class character : public entity
{
public:
    virtual ~character() = default;

    sf::Vector2f heading;

protected:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    // template<typename Projectile>
    // void add_projectile(
    //     scene::projectiles& layer,
    //     sf::Vector2f const& offset,
    //     float const& dir) const
    // {
    //     auto p = std::make_unique<Projectile>();

    //     sf::Vector2f const o{offset.x * sprite.getGlobalBounds().width,
    //                          offset.y * sprite.getGlobalBounds().height};

    //     p->setPosition(world_position() + o * dir);
    //     if(dir == projectile::downward)
    //     {
    //         p->setRotation(180.f);
    //     }
    //     p->velocity = sf::Vector2f{0, p->speed} * dir;

    //     layer.attach(std::move(p));
    // }

    // int const starting_life;
    // int life;

    // lifebar *bar;
};

}
