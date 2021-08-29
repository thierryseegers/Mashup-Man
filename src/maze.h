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
        std::array<std::array<int, level::width>, level::height> const& texture_offsets,
        std::array<std::array<int, level::width>, level::height> const& texture_rotations = {});

    virtual void draw_self(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

private:
    sf::VertexArray vertices;
    sf::Texture texture_strip;
};