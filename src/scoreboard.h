#pragma once

#include <SFML/Graphics.hpp>

class scoreboard
    : public sf::Drawable
    , public sf::Transformable
{
public:
    scoreboard();

    virtual ~scoreboard() = default;

    // Will adjust the marquees' positions.
    void set_size(
        unsigned int const height_,
        unsigned int const width_);

    void increase_score(
        int const player,
        int const amount);

private:
    virtual void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    sf::Text label_1, label_2, label_hi, score_1, score_2, score_hi;

    unsigned int width;
};
