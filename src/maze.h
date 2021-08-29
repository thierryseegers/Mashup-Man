#pragma once

#include "level.h"
#include "scene.h"

#include <SFML/Graphics.hpp>

class maze
    : public scene::node
{
public:
    maze();

    void layout(
        level::wall_texture_offsets const& offsets,
        level::wall_rotations const& rotations = {});

    virtual void draw_self(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

private:
    sf::VertexArray vertices;
    sf::Texture texture_strip;
};