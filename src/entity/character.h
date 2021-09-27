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

class character
    : public entity
{
public:
    using entity::entity;

    virtual ~character() = default;

    // A character may be impervious to damage (e.g. recently spawned hero, dead enemy going home).
    [[nodiscard]] virtual bool immune() const = 0;

protected:
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
        p->throttle(1.f);

        return p;
    }
};

}
