#pragma once

#include "scene.h"
#include "tile_map.h"

class maze
    : public scene::node
{
public:
    maze();

    virtual void draw_self(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

private:
    std::array<std::array<char, 28>, 33> level;
    tile_map walls;
};