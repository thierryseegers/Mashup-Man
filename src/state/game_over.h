#pragma once

#include "state/stack.h"
#include "state/state.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace state
{

class game_over
    : public state
{
public:
    game_over(
        stack& states);

    ~game_over() = default;

    virtual void draw() override;

    virtual bool update(
        sf::Time const& dt) override;

    virtual bool handle_event(
        sf::Event const& event) override;

private:
    sf::Text text;
    sf::Time duration;
    sf::Time elapsed;
};

}
