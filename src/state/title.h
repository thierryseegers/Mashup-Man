#pragma once

#include "state/stack.h"
#include "state/state.h"

#include <SFML/Graphics/RoundedRectangleShape.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace state
{

class title : public state
{
public:
    title(
        stack& states);

    virtual void draw() override;

    virtual bool update(
        sf::Time const& dt) override;

    virtual bool handle_event(
        sf::Event const& event) override;

private:
    static bool scroll; // Whether to scrool the title screen or not. Only the first one ever will.

    sf::View view;
    float to_scroll;

    sf::Text title_;
    sf::RoundedRectangleShape frame;
    sf::RoundedRectangleShape outer_frame;
    sf::Text choices;
    sf::Text arrow;

    int num_players;
};

}
