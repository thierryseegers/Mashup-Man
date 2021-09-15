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
{
    text.setFont(resources::fonts().get(resources::font::main));

    if(states.context.player_1.level_outcome() == player::outcome::failure)
        text.setString("Game over!");	
    else
        text.setString("Get ready for the next level!");

    text.setCharacterSize(100);
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
    // Show state for 3 seconds, after return to menu
    elapsed += dt;
    if(elapsed > sf::seconds(3))
    {
        states.request_clear();
        // states.request_push(id::menu);
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
