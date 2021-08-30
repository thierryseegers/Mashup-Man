#pragma once

#include "action.h"

#include <SFML/Window/Keyboard.hpp>

#include <map>

namespace bindings
{

using keyboard_bindings_t = std::map<sf::Keyboard::Key, action>;
using joystick_bindings_t = std::map<unsigned int, action>;

keyboard_bindings_t& keyboard();
joystick_bindings_t& joystick();

void save();

void bind_key(
    sf::Keyboard::Key const key,
    action const what);
void bind_button(
    unsigned int const button,
    action const what);

sf::Keyboard::Key bound_key(
    action const what);
unsigned int bound_button(
    action const what);
}
