#include "maze.h"

#include "resources.h"
#include "utility.h"

#include <fstream>

maze::maze()
{
    // Load the walls's tile strip.
    walls.load(utility::single::instance<resources::textures_t>().get(resources::texture::walls));

    // Read in the maze tile information.
    std::ifstream file{"assets/levels/1.txt"};
    std::string line;
    for(auto& row : level)
    {
        std::getline(file, line);
        std::copy(line.begin(), line.end(), row.begin());
    }

    // Read in wall tile rotation information.
    std::array<std::array<int, 28>, 33> wall_tile_rotations;
    for(auto& row : wall_tile_rotations)
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
    std::array<std::array<int, 28>, 33> wall_tiles;
    for(size_t r = 0; r != 33; ++r)
    {
        for(size_t c = 0; c != 28; ++c)
        {
            switch(level[r][c])
            {
                case '0':
                case '1':
                case '2':
                case '3':
                    wall_tiles[r][c] = level[r][c] - '0';
                    break;
                default:
                    wall_tiles[r][c] = 4;
                    break;
            }
        }
    }

    walls.layout(wall_tiles, wall_tile_rotations);
}

void maze::draw_self(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    target.draw(walls, states);
}