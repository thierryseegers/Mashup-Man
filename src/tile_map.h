#pragma once

#include <SFML/Graphics.hpp>

#include <array>

class tile_map : 
    public sf::Drawable,
    public sf::Transformable
{
public:
    tile_map();

    void load(
        sf::Texture const& texture_strip);

    template<size_t Width, size_t Height>
    void layout(
        std::array<std::array<int, Width>, Height> const& texture_offsets,
        std::array<std::array<int, Width>, Height> const& texture_rotations = {})
    {
        assert(texture_strip.getSize().y != 0);

        // Resize the vertex array to fit the level size.
        vertices.resize(Width * Height * 4);

        unsigned int const tile_size = texture_strip.getSize().y;

        // Populate the vertex array, with one quad per tile.
        for (unsigned int r = 0; r < Height; ++r)
        {
            for (unsigned int c = 0; c < Width; ++c)
            {
                // Get the current tile offset.
                int const offset = texture_offsets[r][c];

                // Find its position in the texture_strip texture.
                int const tu = offset;
                int const tv = 0;

                // Get a pointer to the current tile's quad.
                sf::Vertex* const quad = &vertices[(c + r * Width) * 4];

                // Define its four corners.
                quad[0].position = sf::Vector2f(c * tile_size, r * tile_size);
                quad[1].position = sf::Vector2f((c + 1) * tile_size, r * tile_size);
                quad[2].position = sf::Vector2f((c + 1) * tile_size, (r + 1) * tile_size);
                quad[3].position = sf::Vector2f(c * tile_size, (r + 1) * tile_size);

                // Define its texture coordinates.
                switch(texture_rotations[r][c])
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

private:
    virtual void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const;

    sf::VertexArray vertices;
    sf::Texture texture_strip;
};
