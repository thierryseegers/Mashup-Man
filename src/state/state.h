#pragma once

#include "state/stack.h"

#include <SFML/System.hpp>

namespace state
{

class state
{
public:
    state(
        stack& states)
        : states{states}
    {}

    virtual ~state() = default;

    virtual void draw() = 0;
    virtual bool update(sf::Time const& dt) = 0;
    virtual bool handle_event(sf::Event const& event) = 0;

protected:
    stack& states;
};

}
