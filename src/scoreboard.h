#pragma once

#include <SFML/Graphics.hpp>

class scoreboard
    : public sf::Drawable
    , public sf::Transformable
{
public:
    scoreboard(
        unsigned int const score_1,
        unsigned int const score_2);

    virtual ~scoreboard() = default;

    // Will adjust the marquees' positions.
    void set_size(
        unsigned int const height_,
        unsigned int const width_);

    void set_score(
        unsigned int const player,
        unsigned int const score);

private:
    virtual void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const override;

    unsigned int score_n_1, score_n_2;
    sf::Text label_1, label_2, label_hi, score_1, score_2, score_hi;

    unsigned int width;
};
