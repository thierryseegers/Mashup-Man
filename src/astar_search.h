#pragma once

#include "direction.h"
#include "level.h"

#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/grid_graph.hpp>
#include <SFML/System/Vector2.hpp>

namespace astar
{

namespace
{

using grid = boost::grid_graph<2, int>;

struct vertex_hash
{
    using argument_type = grid::vertex_descriptor;
    using result_type = std::size_t;

    std::size_t operator()(
        grid::vertex_descriptor const& u) const;
};

using vertex_set = boost::unordered_set<grid::vertex_descriptor, vertex_hash>;
using filtered_grid = boost::vertex_subset_complement_filter<grid, vertex_set>::type;

}

class maze
{
public:
    maze(
        level::info const& level_info);

    // Give the direction to go in given target coordinates.
    direction route(
        grid::vertex_descriptor const& start,
        grid::vertex_descriptor const& goal,
        level::grid<bool>* path);

private:
    // The grid underlying the maze.
    grid grid_;

    // The barriers in the maze.
    vertex_set barriers;

    // The underlying maze grid with barrier vertices filtered out.
    filtered_grid paths;
};

grid::vertex_descriptor to_vertex_descriptor(
    sf::Vector2i const& coordinates);

}
