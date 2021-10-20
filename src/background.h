#pragma once

#include "scene.h"

#include <SFML/Graphics.hpp>

class background
    : public scene::node
{
public:
    background();

private:
    virtual void draw_self(
        sf::RenderTarget& target,
        sf::RenderStates states) const;

    sf::Sprite sprite_;
};