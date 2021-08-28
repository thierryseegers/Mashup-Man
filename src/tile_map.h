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
        sf::Texture const& tile_strip);

    template<size_t Width, size_t Height>
    void layout(
        std::array<std::array<int, Height>, Width> const& values)
    {
        assert(tile_strip.getSize().y != 0);

        // Resize the vertex array to fit the level size.
        vertices.resize(Width * Height * 4);

        unsigned int const tile_size = tile_strip.getSize().y;

        // Populate the vertex array, with one quad per tile.
        for (unsigned int i = 0; i < Width; ++i)
        {
            for (unsigned int j = 0; j < Height; ++j)
            {
                // Get the current tile value.
                int const value = values[j][i];

                // Find its position in the tile_strip texture.
                int const tu = value;
                int const tv = 0;

                // Get a pointer to the current tile's quad.
                sf::Vertex* const quad = &vertices[(i + j * Width) * 4];

                // Define its 4 four corners.
                quad[0].position = sf::Vector2f(i * tile_size, j * tile_size);
                quad[1].position = sf::Vector2f((i + 1) * tile_size, j * tile_size);
                quad[2].position = sf::Vector2f((i + 1) * tile_size, (j + 1) * tile_size);
                quad[3].position = sf::Vector2f(i * tile_size, (j + 1) * tile_size);

                // Define its four texture coordinates.
                quad[0].texCoords = sf::Vector2f(tu * tile_size, tv * tile_size);
                quad[1].texCoords = sf::Vector2f((tu + 1) * tile_size, tv * tile_size);
                quad[2].texCoords = sf::Vector2f((tu + 1) * tile_size, (tv + 1) * tile_size);
                quad[3].texCoords = sf::Vector2f(tu * tile_size, (tv + 1) * tile_size);
            }
        }
    }

private:
    virtual void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const;

    sf::VertexArray vertices;
    sf::Texture tile_strip;
};
