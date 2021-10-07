#include "astar_route.h"

#include <boost/unordered_map.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/grid_graph.hpp>
#include <spdlog/spdlog.h>

#include <cmath>
#include <memory>

namespace astar
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

using distance = double;

std::size_t vertex_hash::operator()(
    grid::vertex_descriptor const& u) const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, u[0]);
    boost::hash_combine(seed, u[1]);

    return seed;
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
        return sqrt(pow(double(goal[0] - v[0]), 2) + pow(double(goal[1] - v[1]), 2));
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
        if(u == goal)
        {
            throw found_goal();
        }
    }
};

class maze
{
public:
    maze(
        level::description const& level_description)
        : grid_{{{(int)level_description[0].size(), (int)level_description.size()}}}
        , paths{boost::make_vertex_subset_complement_filter(grid_, barriers)}
    {
        for(size_t r = 0; r != level_description.size(); ++r)
        {
            for(size_t c = 0; c != level_description[0].size(); ++c)
            {
                switch(level_description[r][c])
                {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    // case 'p':
                        barriers.insert(vertex(c + r * level_description[0].size(), grid_));
                        break;
                    default:
                        break;
                }
            }
        }
    }

    // Give the direction to go in given target coordinates.
    direction route(
        grid::vertex_descriptor const& start,
        grid::vertex_descriptor const& goal) const
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
            for(u = goal; predecessors[u] != start; u = predecessors[u])
            {}

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
                return direction::up;
            }
            else if(u[1] - 1 == start[1])
            {
                return direction::down;
            }
        }

        return direction::none;
    }

private:
    // The grid underlying the maze.
    grid grid_;

    // The barriers in the maze.
    vertex_set barriers;

    // The underlying maze grid with barrier vertices filtered out.
    filtered_grid paths;
};

grid::vertex_descriptor to_vertex_descriptor(
    sf::Vector2i const& coordinates)
{
    return {{static_cast<grid::vertices_size_type>(coordinates.x), static_cast<grid::vertices_size_type>(coordinates.y)}};
}

std::shared_ptr<maze> make_maze(
    level::description const& level_description)
{
    return std::make_shared<maze>(level_description);
}

direction route(
    maze const* const maze_,
    sf::Vector2i const& start,
    sf::Vector2i const& goal)
{
    return maze_->route(to_vertex_descriptor(start), to_vertex_descriptor(goal));
}

}