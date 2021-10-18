#pragma once

#include <SFML/System/Vector2.hpp>

enum class direction
{
    up,
    down,
    left,
    right,
    none
};

// Returns the opposite direction or `none` if `none`.
direction operator~(
    direction const& d);

// Returns a unit vector of the given direction or `{0.f, 0.f}` if `none`.
sf::Vector2f to_vector2f(
    direction const d);

// Returns a unit vector of the given direction or `{0, 0}` if `none`.
sf::Vector2i to_vector2i(
    direction const d);
