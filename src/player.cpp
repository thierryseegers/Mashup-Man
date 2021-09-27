#include "player.h"

#include "action.h"
#include "bindings.h"
#include "command.h"
#include "direction.h"
#include "entity/entity.h"
#include "entity/hero.h"
#include "utility.h"

#include <SFML/Window.hpp>

#include <map>

player::hero_maker_f player::hero_maker() const
{
    return hero_maker_;
}

void player::handle_event(
    sf::Event const& event,
    commands_t& commands)
{
    if(event.type == sf::Event::KeyPressed)
    {
        if(auto i = bindings::keyboard(id).find(event.key.code); i != bindings::keyboard(id).end() && !is_realtime_action(i->second))
        {
            commands.push(action_bindings[i->second]);
        }
    }
    else if(event.type == sf::Event::KeyReleased)
    {
        if(auto i = bindings::keyboard(id).find(event.key.code); i != bindings::keyboard(id).end() && 
                                                               !is_realtime_action(i->second) && 
                                                               utility::any_of(i->second, action::head_down, action::head_left, action::head_right, action::head_up))
        {
            commands.push(action_bindings[action::cruise]);
        }
    }
    else if(event.type == sf::Event::JoystickMoved && event.joystickMove.joystickId == id)
    {
        if(auto const x_axis = sf::Joystick::getAxisPosition(id, sf::Joystick::Axis::X); x_axis > 1)
        {
            commands.push(action_bindings[action::head_right]);
        }
        else if(x_axis < -1)
        {
            commands.push(action_bindings[action::head_left]);
        }
        else if(auto const y_axis = sf::Joystick::getAxisPosition(id, sf::Joystick::Axis::Y); y_axis > 1)
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
    else if(event.type == sf::Event::JoystickButtonPressed && event.joystickMove.joystickId == id)
    {
        if(auto i = bindings::joystick().find(event.joystickButton.button); i != bindings::joystick().end() && !is_realtime_action(i->second))
        {
            commands.push(action_bindings[i->second]);
        }
    }
}

void player::handle_realtime_input(
    commands_t& commands)
{
    for(auto const& [key, what] : bindings::keyboard(id))
    {
        if(sf::Keyboard::isKeyPressed(key) &&
           is_realtime_action(what))
        {
            commands.push(action_bindings[what]);
        }
    }

    for(auto const& [button, what] : bindings::joystick())
    {
        if(sf::Joystick::isButtonPressed(id, button) &&
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

bool player::is_realtime_action(
    action const a)
{
    switch(a)
    {
        // case action::cruise:
        // case action::head_down:
        // case action::head_left:
        // case action::head_right:
        // case action::head_up:
        case action::attack:
            return true;
            break;
        default:
            return false;
            break;
    }
}