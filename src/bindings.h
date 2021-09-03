#pragma once

#include "action.h"

#include <SFML/Window/Keyboard.hpp>

#include <map>
#include <string>

namespace bindings
{

using keyboard_bindings_t = std::map<sf::Keyboard::Key, action>;
using joystick_bindings_t = std::map<unsigned int, action>;

keyboard_bindings_t& keyboard(std::string const&);
joystick_bindings_t& joystick();

void save();

void bind_key(
    std::string const& name,
    sf::Keyboard::Key const key,
    action const what);
void bind_button(
    unsigned int const button,
    action const what);

sf::Keyboard::Key bound_key(
    std::string const& name,
    action const what);
unsigned int bound_button(
    action const what);
}
