#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

class lifeboard
    : public sf::Drawable
    , public sf::Transformable
{
public:
    struct info
    {
        sf::Sprite sprite;
        int lives;
    };

    virtual ~lifeboard() = default;

    info& operator[](
        size_t const i);

    virtual void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

private:
    mutable std::vector<info> infos;
};
