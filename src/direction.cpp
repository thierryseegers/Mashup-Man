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
}

sf::Vector2f to_vector(
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
}
