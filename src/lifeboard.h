#pragma once

#include <SFML/Graphics.hpp>

class lifeboard
    : public sf::Drawable
    , public sf::Transformable
{
public:
    lifeboard(
        sf::Sprite player_1,
        sf::Sprite player_2);

    virtual ~lifeboard() = default;

    int& operator[](int i);

    virtual void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

private:
    mutable struct info
    {
        sf::Sprite sprite;
        int lives;
    } player_1, player_2;
};
