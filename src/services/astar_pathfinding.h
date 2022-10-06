#pragma once

#include "direction.h"
#include "level.h"

#include <SFML/System/Vector2.hpp>

// #include <experimental/propagate_const>
#include <memory>

namespace services::pathfinding
{

class astar
{
public:
    astar() noexcept;

    ~astar() noexcept;

    // Set the map to use when finding a route.
    void map(
        level::description const& level_description) noexcept;

    // Returns the direction to take as the next to get the goal.
    direction route(
        sf::Vector2i const& start,
        sf::Vector2i const& goal) const noexcept;

private:
    class impl;
    // std::experimental::propagate_const<std::unique_ptr<impl>> pimpl;
    std::unique_ptr<impl> pimpl;
};

}
