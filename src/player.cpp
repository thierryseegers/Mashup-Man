#include "player.h"

#include "action.h"
#include "bindings.h"
#include "command.h"
#include "entity/brother.h"
// #include "entity/leader.h"

#include <SFML/Window.hpp>

#include <map>

player_t::player_t()
    // : status{mission::running}
{
    float const speed = 50.f;

    action_bindings[action::go_down] = make_command<entity::brother>([=](entity::brother& bro, sf::Time const&)
        {
            bro.velocity = {0.f, speed};
        });
    action_bindings[action::go_left] = make_command<entity::brother>([=](entity::brother& bro, sf::Time const&)
        {
            bro.velocity = {-speed, 0.f};
        });
    action_bindings[action::go_right] = make_command<entity::brother>([=](entity::brother& bro, sf::Time const&)
        {
            bro.velocity = {speed, 0.f};
        });
    action_bindings[action::go_up] = make_command<entity::brother>([=](entity::brother& bro, sf::Time const&)
        {
            bro.velocity = {0.f, -speed};
        });

    action_bindings[action::fire] = make_command<entity::brother>([=](entity::brother& bro, sf::Time const&)
        {
            bro.fire();
        });
}

void player_t::handle_event(
    sf::Event const& event,
    commands_t& commands)
{
    if(event.type == sf::Event::KeyPressed)
    {
        if(auto i = bindings::keyboard().find(event.key.code); i != bindings::keyboard().end() && !is_realtime_action(i->second))
        {
            commands.push(action_bindings[i->second]);
        }
    }
    else if(event.type == sf::Event::JoystickButtonPressed)
    {
        if(auto i = bindings::joystick().find(event.joystickButton.button); i != bindings::joystick().end() && !is_realtime_action(i->second))
        {
            commands.push(action_bindings[i->second]);
        }
    }
}

void player_t::handle_realtime_input(
    commands_t& commands)
{
    for(auto const& [key, what] : bindings::keyboard())
    {
        if(sf::Keyboard::isKeyPressed(key) &&
           is_realtime_action(what))
        {
            commands.push(action_bindings[what]);
        }
    }

    for(auto const& [button, what] : bindings::joystick())
    {
        if(sf::Joystick::isButtonPressed(0, button) &&
           is_realtime_action(what))
        {
            commands.push(action_bindings[what]);
        }
    }

    if(auto const x_axis = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X); x_axis > 1)
    {
        commands.push(action_bindings[action::go_right]);
    }
    else if(x_axis < -1)
    {
        commands.push(action_bindings[action::go_left]);
    }

    if(auto const y_axis = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y); y_axis > 1)
    {
        commands.push(action_bindings[action::go_down]);
    }
    else if(y_axis < -1)
    {
        commands.push(action_bindings[action::go_up]);
    }
}

bool player_t::is_realtime_action(
    action const a)
{
    switch(a)
    {
        case action::go_down:
        case action::go_left:
        case action::go_right:
        case action::go_up:
        case action::fire:
            return true;
            break;
        default:
            return false;
            break;
    }
}