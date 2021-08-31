#include "state/game.h"

// #include "music.h"
#include "state/stack.h"

#include <SFML/System.hpp>

namespace state
{

game::game(
    stack& states)
    : state{states}
    , world{states.context.window, states.context.sound}
{
    // states.context.music.play(music::theme::mission);
}

void game::draw()
{
    world.draw();
}

bool game::update(
    sf::Time const& dt)
{
    world.update(dt);

    // if(!world.player_alive())
    // {
    //     states.context.player.mission_status() = player_t::mission::failure;
    //     states.request_push(id::game_over);
    // }
    // else if(world.player_reached_end())
    // {
    //     states.context.player.mission_status() = player_t::mission::success;
    //     states.request_push(id::game_over);
    // }

    states.context.player.handle_realtime_input(world.commands());

    return true;
}

bool game::handle_event(
    sf::Event const& event)
{
    // Game input handling.
    states.context.player.handle_event(event, world.commands());

    // // Escape key or Start button pressed, trigger the pause screen.
    // if(event.type == sf::Event::KeyReleased &&
    //    event.key.code == sf::Keyboard::Escape)
    // {
    //     states.request_push(id::pause);
    // }
    // else if(event.type == sf::Event::JoystickButtonReleased &&
    //         event.joystickButton.button == 6)
    // {
    //     states.request_push(id::pause);
    // }

    return true;
}

}
