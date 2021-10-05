#pragma once

#include "direction.h"
#include "level.h"

#include <SFML/System/Vector2.hpp>

#include <memory>

namespace astar
{

class maze;

std::shared_ptr<maze> make_maze(
    level::description const& level_description);

direction route(
    maze const* const maze_,
    sf::Vector2i const& start,
    sf::Vector2i const& goal);

}
