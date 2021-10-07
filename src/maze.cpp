#include "maze.h"

#include "level.h"
#include "resources.h"
#include "utility.h"

#include <filesystem>
#include <map>

constexpr maze::structure to_structure(
    char const c)
{
    switch(c)
    {
        case '0':
        case '1':
        case '2':
        case '3':
            return maze::wall;
        case 'p':
            return maze::pipe;
        case 'd':
            return maze::door;
        default:
            return maze::path;
    }
}


maze::maze(
    std::filesystem::path const& level)
    : texture_strip{resources::textures().get(resources::texture::walls)}
{
    vertices.setPrimitiveType(sf::Quads);

    // Resize the vertex array to fit the level size.
    vertices.resize(level::width * level::height * 4);

    level::grid<int> wall_texture_offsets;
    level::grid<int> wall_texture_rotations;

    // Read in the maze tile information.
    std::ifstream file{level.c_str()};
    std::string line;
    for(auto& row : level_description)
    {
        std::getline(file, line);
        std::copy(line.begin(), line.end(), row.begin());
    }

    // Read in wall tile rotation information.
    for(auto& row : wall_texture_rotations)
    {
        size_t column = 0;
        std::getline(file, line);
        std::for_each(line.begin(), line.end(), [&](char const c)
        {
            switch(c)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                    row[column] = c - '0';
                    break;
                default:
                    row[column] = 0;
                    break;
            }
            ++column;
        });
        column = 0;
    }

    // Extract just the wall tile information.
    for(size_t r = 0; r != level::height; ++r)
    {
        for(size_t c = 0; c != level::width; ++c)
        {
            switch(level_description[r][c])
            {
                case '0':
                case '1':
                case '2':
                case '3':
                    wall_texture_offsets[r][c] = level_description[r][c] - '0';
                    break;
                default:
                    wall_texture_offsets[r][c] = 4;
                    break;
            }
        }
    }

    // Populate the vertex array, with one quad per tile.
    unsigned int const tile_size = texture_strip.getSize().y;

    for (unsigned int r = 0; r < level::height; ++r)
    {
        for (unsigned int c = 0; c < level::width; ++c)
        {
            // Get the current tile offset.
            int const offset = wall_texture_offsets[r][c];

            // Find its position in the texture_strip texture.
            int const tu = offset;
            int const tv = 0;

            // Get a pointer to the current tile's quad.
            sf::Vertex* const quad = &vertices[(c + r * level::width) * 4];

            // Define its four corners.
            quad[0].position = sf::Vector2f(c * tile_size, r * tile_size);
            quad[1].position = sf::Vector2f((c + 1) * tile_size, r * tile_size);
            quad[2].position = sf::Vector2f((c + 1) * tile_size, (r + 1) * tile_size);
            quad[3].position = sf::Vector2f(c * tile_size, (r + 1) * tile_size);

            // Define its texture coordinates.
            switch(wall_texture_rotations[r][c])
            {
                default:
                case 0: // No rotation.
                    quad[0].texCoords = sf::Vector2f(tu * tile_size, tv * tile_size);
                    quad[1].texCoords = sf::Vector2f((tu + 1) * tile_size, tv * tile_size);
                    quad[2].texCoords = sf::Vector2f((tu + 1) * tile_size, (tv + 1) * tile_size);
                    quad[3].texCoords = sf::Vector2f(tu * tile_size, (tv + 1) * tile_size);
                    break;
                case 1: // 90 degrees clockwise rotation.
                    quad[0].texCoords = sf::Vector2f(tu * tile_size, (tv + 1) * tile_size);
                    quad[1].texCoords = sf::Vector2f(tu * tile_size, tv * tile_size);
                    quad[2].texCoords = sf::Vector2f((tu + 1) * tile_size, tv * tile_size);
                    quad[3].texCoords = sf::Vector2f((tu + 1) * tile_size, (tv + 1) * tile_size);
                    break;
                case 2: // 180 degrees clockwise rotation.
                    quad[0].texCoords = sf::Vector2f((tu + 1) * tile_size, (tv + 1) * tile_size);
                    quad[1].texCoords = sf::Vector2f(tu * tile_size, (tv + 1) * tile_size);
                    quad[2].texCoords = sf::Vector2f(tu * tile_size, tv * tile_size);
                    quad[3].texCoords = sf::Vector2f((tu + 1) * tile_size, tv * tile_size);
                    break;
                case 3: // 270 degrees clockwise rotation.
                    quad[0].texCoords = sf::Vector2f((tu + 1) * tile_size, tv * tile_size);
                    quad[1].texCoords = sf::Vector2f((tu + 1) * tile_size, (tv + 1) * tile_size);
                    quad[2].texCoords = sf::Vector2f(tu * tile_size, (tv + 1) * tile_size);
                    quad[3].texCoords = sf::Vector2f(tu * tile_size, tv * tile_size);
                    break;
            }
        }
    }

    astar_maze = astar::make_maze(level_description);
}

char maze::operator[](
    sf::Vector2i const& coordinates) const
{
    return level_description[coordinates.y][coordinates.x];
}

std::map<direction, maze::structure> maze::around(
    sf::Vector2i const& coordinates) const
{
    return {{direction::down,   to_structure(operator[]({coordinates.x, coordinates.y + 1}))},
            {direction::left,   to_structure(operator[]({coordinates.x - 1, coordinates.y}))},
            {direction::right,  to_structure(operator[]({coordinates.x + 1, coordinates.y}))},
            {direction::up,     to_structure(operator[]({coordinates.x, coordinates.y - 1}))}};
}

direction maze::route(
    sf::Vector2i const& start,
    sf::Vector2i const& goal) const
{
    return astar::route(astar_maze.get(), start, goal);
}

void maze::draw_self(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &texture_strip;
    target.draw(vertices, states);
}
