#include "direction.h"

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