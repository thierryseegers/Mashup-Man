#pragma once

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
    enum class structure
    {
        path,
        pipe,
        wall,
        door
    };

    static constexpr structure to_structure(
        char const c)
    {
        switch(c)
        {
            case '0':
            case '1':
            case '2':
            case '3':
                return maze::structure::wall;
            case 'w':
                return maze::structure::pipe;
            case 's':
                return maze::structure::door;
            default:
                return maze::structure::path;
        }
    }

    maze(
        std::filesystem::path const& level);

    // Returns the level code at a given cell.
    char operator[](
        sf::Vector2i const& coordinates) const;

    // Returns the structures that surround a cell.
    std::map<direction, char> around(
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
};