#include "game_over.h"

#include "state/stack.h"
#include "state/state.h"
#include "player.h"
#include "resources.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace state
{

game_over::game_over(
    stack& states)
    : state{states}
    , text{"Game over!", resources::fonts().get(resources::font::main), 100}
    , duration{resources::sound_buffers().get(resources::sound_effect::game_over).getDuration() + sf::seconds(3)}
    , elapsed{sf::Time::Zero}
{
    utility::center_origin(text);

    auto const view_size = states.context.window.getView().getSize();
    text.setPosition(view_size.x / 2, view_size.y / 2);
}

void game_over::draw()
{
    sf::RenderWindow& window = states.context.window;

    // Create dark, semi-transparent background.
    sf::RectangleShape background;
    background.setFillColor(sf::Color(0, 0, 0, 150));
    background.setSize(window.getView().getSize());

    window.draw(background);
    window.draw(text);
}

bool game_over::update(
    sf::Time const& dt)
{
    // Show state for a set time before returning to title screen.
    elapsed += dt;
    if(elapsed > duration)
    {
        states.request_clear();
        states.request_push(id::title);
    }

    return false;
}

bool game_over::handle_event(
    sf::Event const& event)
{
    if(event.type == sf::Event::Resized)
    {
        auto const view_size = states.context.window.getView().getSize();
        text.setPosition(view_size.x / 2, view_size.y / 2);

        return true;
    }

    return false;
}

}
