#pragma once
#pragma once

#include "state/stack.h"
#include "state/state.h"
#include "world.h"

#include <SFML/System.hpp>

namespace state
{

class game :
    public state
{
public:
    game(
        stack& states);

    virtual void draw() override;

    virtual bool update(
        sf::Time const& dt) override;

    virtual bool handle_event(
        sf::Event const& event) override;

private:
    world_t world;
};

}
