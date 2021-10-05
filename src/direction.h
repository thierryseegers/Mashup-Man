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

sf::Vector2f to_vector(
    direction const d);
