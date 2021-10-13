#pragma once

#include "astar_route.h"
#include "direction.h"
#include "level.h"
#include "scene.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include <filesystem>
#include <map>

class maze
    : public scene::node
{
public:
    enum structure
    {
        path    = 0x1,
        pipe    = 0x2,
        wall    = 0x4,
        door    = 0x8
    };

    maze(
        std::filesystem::path const& level);

    // Returns the level code at a given cell.
    char operator[](
        sf::Vector2i const& coordinates) const;

    // Returns the structures that surround a cell.
    std::map<direction, structure> around(
        sf::Vector2i const& coordinates) const;

    // Returns the coordinates of the ghost house.
    sf::IntRect ghost_house() const;

    // Returns the direction to go to next on the path from `start` to `goal`.
    direction route(
        sf::Vector2i const& start,
        sf::Vector2i const& goal) const;

private:
    virtual void draw_self(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    level::description level_description;
    sf::VertexArray vertices;
    sf::Texture texture_strip;

    sf::IntRect ghost_house_;

    std::shared_ptr<astar::maze> astar_maze;
};