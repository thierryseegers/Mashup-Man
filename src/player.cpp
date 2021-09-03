#include "player.h"

#include "action.h"
#include "bindings.h"
#include "command.h"
#include "direction.h"
#include "entity/entity.h"
#include "entity/brother.h"
#include "utility.h"

#include <SFML/Window.hpp>

#include <map>

player_t::player_t()
    // : status{mission::running}
{
    action_bindings[action::cruise] = make_command<entity::brother>([=](entity::brother& bro, sf::Time const&)
        {
            bro.face(direction::still);
        });
    action_bindings[action::head_down] = make_command<entity::brother>([=](entity::brother& bro, sf::Time const&)
        {
            bro.face(direction::down);
        });
    action_bindings[action::head_left] = make_command<entity::brother>([=](entity::brother& bro, sf::Time const&)
        {
            bro.face(direction::left);
        });
    action_bindings[action::head_right] = make_command<entity::brother>([=](entity::brother& bro, sf::Time const&)
        {
            bro.face(direction::right);
        });
    action_bindings[action::head_up] = make_command<entity::brother>([=](entity::brother& bro, sf::Time const&)
        {
            bro.face(direction::up);
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
    else if(event.type == sf::Event::KeyReleased)
    {
        if(auto i = bindings::keyboard().find(event.key.code); i != bindings::keyboard().end() && 
                                                               !is_realtime_action(i->second) && 
                                                               utility::any_of(i->second, action::head_down, action::head_left, action::head_right, action::head_up))
        {
            commands.push(action_bindings[action::cruise]);
        }
    }
    else if(event.type == sf::Event::JoystickMoved)
    {
        if(auto const x_axis = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X); x_axis > 1)
        {
            commands.push(action_bindings[action::head_right]);
        }
        else if(x_axis < -1)
        {
            commands.push(action_bindings[action::head_left]);
        }
        else if(auto const y_axis = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y); y_axis > 1)
        {
            commands.push(action_bindings[action::head_down]);
        }
        else if(y_axis < -1)
        {
            commands.push(action_bindings[action::head_up]);
        }
        else
        {
            commands.push(action_bindings[action::cruise]);
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

    // if(auto const x_axis = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X); x_axis > 1)
    // {
    //     commands.push(action_bindings[action::head_right]);
    // }
    // else if(x_axis < -1)
    // {
    //     commands.push(action_bindings[action::head_left]);
    // }

    // if(auto const y_axis = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y); y_axis > 1)
    // {
    //     commands.push(action_bindings[action::head_down]);
    // }
    // else if(y_axis < -1)
    // {
    //     commands.push(action_bindings[action::head_up]);
    // }
}

bool player_t::is_realtime_action(
    action const a)
{
    switch(a)
    {
        // case action::cruise:
        // case action::head_down:
        // case action::head_left:
        // case action::head_right:
        // case action::head_up:
        case action::fire:
            return true;
            break;
        default:
            return false;
            break;
    }
}