#pragma once

#include "command.h"
#include "direction.h"
#include "entity/entity.h"
#include "layer.h"
#include "resources.h"
#include "scene.h"
#include "sprite.h"

#include <SFML/System/Vector2.hpp>

#include <utility>

namespace entity
{

class character : public entity
{
public:
    character(
        sprite sprite_,
        float const speed,
        direction const facing_,
        direction const heading_ = direction::still);

    virtual ~character() = default;

    direction facing() const;

    void face(
        direction const d);

protected:
    direction facing_;

    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    template<typename Projectile>
    Projectile* add_projectile(
        layer::projectiles& layer,
        sf::Vector2f const& position,
        direction const heading) const
    {
        auto p = layer.attach<Projectile>(heading);

        p->setPosition(position);

        return p;
    }
};

}
