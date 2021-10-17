#include "direction.h"

direction operator~(
    direction const& d)
{
    switch(d)
    {
        case direction::down:
            return direction::up;
        case direction::left:
            return direction::right;
        case direction::none:
            return direction::none;
        case direction::right:
            return direction::left;
        case direction::up:
            return direction::down;
    }

    return direction::none;
}

sf::Vector2f to_vector2f(
    direction const d)
{
    switch(d)
    {
        case direction::down:
            return sf::Vector2f{0.f, 1.f};
        case direction::left:
            return sf::Vector2f{-1.f, 0.f};
        case direction::none:
            return sf::Vector2f{0.f, 0.f};
        case direction::right:
            return sf::Vector2f{1.f, 0.f};
        case direction::up:
            return sf::Vector2f{0.f, -1.f};
    }

    return sf::Vector2f{0.f, 0.f};
}

sf::Vector2i to_vector2i(
    direction const d)
{
    switch(d)
    {
        case direction::down:
            return sf::Vector2i{0, 1};
        case direction::left:
            return sf::Vector2i{-1, 0};
        case direction::none:
            return sf::Vector2i{0, 0};
        case direction::right:
            return sf::Vector2i{1, 0};
        case direction::up:
            return sf::Vector2i{0, -1};
    }

    return sf::Vector2i{0, 0};
}