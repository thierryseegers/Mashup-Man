#pragma once

#include "entity/entity.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <map>

namespace entity
{

class points
    : public entity
{
public:
    points(
        unsigned int const number);

protected:
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands) override;

    static std::map<unsigned int, sf::RenderTexture> number_textures;

    sf::Time duration_;
};

}
