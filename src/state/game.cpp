#include "state/game.h"

// #include "music.h"
#include "state/stack.h"

#include <SFML/System.hpp>

namespace state
{

game::game(
    stack& states)
    : state{states}
    , world_{states.context}
{}

void game::draw()
{
    world_.draw();
}

bool game::update(
    sf::Time const& dt)
{
    world_.update(dt);

    if(world_.players_done())
    {
        if(world_.players_alive())
        {
            // states.request_push(id::next_level);
        }
        else 
        {
            states.request_push(id::game_over);
        }
    }

    for(auto& player : states.context.players)
    {
        player->handle_realtime_input(world_.commands());
    }

    return true;
}

bool game::handle_event(
    sf::Event const& event)
{
    // Game input handling.
    for(auto& player : states.context.players)
    {
        player->handle_event(event, world_.commands());
    }

    if(event.type == sf::Event::Resized)
    {
        world_.handle_size_changed(event.size);
    }

    // Escape key or Start button pressed, trigger the pause screen.
    if(event.type == sf::Event::KeyReleased &&
       event.key.code == sf::Keyboard::Escape)
    {
        states.request_push(id::pause);
    }
    else if(event.type == sf::Event::JoystickButtonReleased &&
            event.joystickButton.button == 6)
    {
        states.request_push(id::pause);
    }

    return true;
}

}
