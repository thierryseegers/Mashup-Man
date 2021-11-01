#pragma once

#include "state/stack.h"
#include "state/state.h"
#include "sprite.h"

#include <SFML/System.hpp>

#include <memory>

namespace state
{

class level_complete
    : public state
{
public:
    level_complete(
        stack& states);

    virtual void draw() override;

    virtual bool update(
        sf::Time const& dt) override;

    virtual bool handle_event(
        sf::Event const& event) override;

private:
    sf::Text text;
    sf::Time duration;
    sf::Time elapsed;

    int fireworks;
    std::unique_ptr<sprite> explosion;
};

}
