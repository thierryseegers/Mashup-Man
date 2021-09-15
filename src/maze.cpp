#include "maze.h"

#include "level.h"
#include "resources.h"
#include "utility.h"

maze::maze()
    : texture_strip{resources::textures().get(resources::texture::walls)}
{
    vertices.setPrimitiveType(sf::Quads);

    // Resize the vertex array to fit the level size.
    vertices.resize(level::width * level::height * 4);
}

void maze::layout(
    level::wall_texture_offsets const& offsets,
    level::wall_rotations const& rotations)
{
    unsigned int const tile_size = texture_strip.getSize().y;

    // Populate the vertex array, with one quad per tile.
    for (unsigned int r = 0; r < level::height; ++r)
    {
        for (unsigned int c = 0; c < level::width; ++c)
        {
            // Get the current tile offset.
            int const offset = offsets[r][c];

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
            switch(rotations[r][c])
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
}

void maze::draw_self(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &texture_strip;
    target.draw(vertices, states);
}