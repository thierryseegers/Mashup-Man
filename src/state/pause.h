#pragma once

#include "state/stack.h"
#include "state/state.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace state
{

class pause
    : public state
{
public:
    pause(
        stack& states);

    ~pause();
    
    virtual void draw() override;

    virtual bool update(
        sf::Time const& dt) override;

    virtual bool handle_event(
        sf::Event const& event) override;

private:
    sf::Text word;
    sf::Text instructions;
};

}
