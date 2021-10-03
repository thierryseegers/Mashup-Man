#include "astar_search.h"

#include <boost/unordered_map.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/grid_graph.hpp>
#include <spdlog/spdlog.h>

#include <cmath>

namespace astar
{

using distance = double;

std::size_t vertex_hash::operator()(
    grid::vertex_descriptor const& u) const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, u[0]);
    boost::hash_combine(seed, u[1]);

    return seed;
}

maze::maze(
    level::info const& level_info)
    : grid_{{{(int)level_info[0].size(), (int)level_info.size()}}}
    , paths{boost::make_vertex_subset_complement_filter(grid_, barriers)}
{
    for(size_t r = 0; r != level_info.size(); ++r)
    {
        for(size_t c = 0; c != level_info[0].size(); ++c)
        {
            switch(level_info[r][c])
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case 'p':
                    barriers.insert(vertex(c + r * level_info[0].size(), grid_));
                    break;
                default:
                    break;
            }
        }
    }
}

struct euclidean_distance
    : public boost::astar_heuristic<filtered_grid, distance>
{
    grid::vertex_descriptor goal;

    euclidean_distance(
        grid::vertex_descriptor const& goal)
        : goal{goal}
    {}

    distance operator()(
        grid::vertex_descriptor const& v)
    {
        auto const x1 = goal[0];
        auto const x2 = v[0];
        auto const y1 = goal[1];
        auto const y2 = v[1];
        double const delta_x = x1 - x2;
        double const delta_y = y1 - y2;
        auto const delta_x_squared = pow(delta_x, 2);
        auto const delta_y_squared = pow(delta_y, 2);
        auto const sqrt_deltas_squared = sqrt(delta_x_squared + delta_y_squared);

        auto const d = sqrt(pow(double(goal[0] - v[0]), 2) + pow(double(goal[1] - v[1]), 2));
        spdlog::info("distance to goal: {}", d);

        // return sqrt(pow(double(goal[0] - v[0]), 2) + pow(double(goal[1] - v[1]), 2));
        return d;
    }
};

struct found_goal
{};

struct astar_goal_visitor
    : public boost::default_astar_visitor
{
    grid::vertex_descriptor goal;

    astar_goal_visitor(
        grid::vertex_descriptor const& goal)
        : goal{goal}
    {}

    void examine_vertex(
        grid::vertex_descriptor const& u,
        filtered_grid const&)
    {
        spdlog::info("visiting [{},{}]", u[0], u[1]);

        if(u == goal)
        {
            throw found_goal();
        }
    }
};

// Give the direction to go in given target coordinates.
direction maze::route(
    grid::vertex_descriptor const& start,
    grid::vertex_descriptor const& goal,
    level::grid<bool>* path)
{
    boost::static_property_map<distance> weight(1);
    
    // The predecessor map is a vertex-to-vertex mapping.
    using predecessor_map = boost::unordered_map<grid::vertex_descriptor, grid::vertex_descriptor, vertex_hash>;
    predecessor_map predecessors;
    boost::associative_property_map<predecessor_map> predecessors_property_map(predecessors);

    euclidean_distance heuristic{goal};
    astar_goal_visitor visitor{goal};

    try
    {
        astar_search(paths, start, heuristic,
            boost::weight_map(weight)
                .predecessor_map(predecessors_property_map)
                .visitor(visitor));
    }
    catch(found_goal const&)
    {
        grid::vertex_descriptor u;
        for(u = goal; u != start; u = predecessors[u])
        {
            spdlog::info("[{},{}]", u[0], u[1]);
            (void)u;
            if(path)
            {
                (*path)[u[1]][u[0]] = true;
            }
        }

        if(u[0] + 1 == start[0])
        {
            return direction::left;
        }
        else if(u[0] - 1 == start[0])
        {
            return direction::right;
        }
        else if(u[1] + 1 == start[1])
        {
            return direction::down;
        }
        else if(u[1] - 1 == start[1])
        {
            return direction::up;
        }
    }

    return direction::none;
}

grid::vertex_descriptor to_vertex_descriptor(
    sf::Vector2i const& coordinates)
{
    return {{static_cast<grid::vertices_size_type>(coordinates.x), static_cast<grid::vertices_size_type>(coordinates.y)}};
}

}