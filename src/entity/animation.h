#pragma once

#include "entity/entity.h"
#include "resources.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

#include <vector>

namespace entity
{

class animation
    : public entity
{
public:
    animation(
        resources::texture const& texture,
        std::vector<sf::IntRect> const& texture_rects,
        sf::Time const duration,
        float const scale_factor = 1.f);

protected:
    void update_self(
        sf::Time const& dt,
        commands_t& commands);

    sf::Time countdown;
};

}
