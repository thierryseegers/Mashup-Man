#pragma once

#include "astar_route.h"
#include "direction.h"
#include "level.h"
#include "scene.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include <filesystem>

class maze
    : public scene::node
{
public:
    enum class structure
    {
        path,
        pipe,
        wall
    };

    maze(
        std::filesystem::path const& level);

    char operator[](
        sf::Vector2i const& coordinates) const;

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

    std::shared_ptr<astar::maze> astar_maze;
};