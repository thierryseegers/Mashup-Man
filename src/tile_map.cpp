#include "tile_map.h"

#include <SFML/Graphics.hpp>

tile_map::tile_map()
{
    vertices.setPrimitiveType(sf::Quads);
}

void tile_map::load(
    sf::Texture const& tile_strip_)
{
    texture_strip = tile_strip_;
}

void tile_map::draw(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &texture_strip;
    target.draw(vertices, states);
}
