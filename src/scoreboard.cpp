#include "scoreboard.h"

#include "level.h"
#include "resources.h"
#include "utility.h"

scoreboard::scoreboard()
    : label_1{"1UP", resources::fonts().get(resources::font::retro)}
    , label_2{"2UP", resources::fonts().get(resources::font::retro)}
    , label_hi{"HI-SCORE", resources::fonts().get(resources::font::retro)}
    , score_1{"00", resources::fonts().get(resources::font::retro)}
    , score_2{"00", resources::fonts().get(resources::font::retro)}
    , score_hi{"10000", resources::fonts().get(resources::font::retro)}
{
    utility::invoke_on([](sf::Text& t){ t.setFillColor({28, 116, 238}); }, label_1, label_hi, label_2);

    label_1.setPosition(0, 0);
    score_1.setPosition(0, label_1.getLocalBounds().height + 5);
}

void scoreboard::set_size(
    unsigned int const height_,
    unsigned int const width_)
{
    width = width_;

    utility::invoke_on([=](sf::Text& t){ t.setCharacterSize((height_ - 5) / 2); }, label_1, label_hi, label_2, score_1, score_hi, score_2);

    score_1.setPosition(0, label_1.getLocalBounds().height + 5);

    label_hi.setPosition((width - label_hi.getLocalBounds().width) / 2, 0);
    score_hi.setPosition((width - score_hi.getLocalBounds().width) / 2, label_1.getLocalBounds().height + 5);

    label_2.setPosition(width - label_2.getLocalBounds().width, 0);
    score_2.setPosition(width - score_2.getLocalBounds().width, label_1.getLocalBounds().height + 5);
}

void scoreboard::increase_score(
    int const player,
    int const amount)
{
    auto const s_1 = std::stoi(score_1.getString().toAnsiString());
    auto const s_2 = std::stoi(score_2.getString().toAnsiString());

    if(player == 0)
    {
        score_1.setString(std::to_string(s_1 + amount));
    }
    else
    {
        score_2.setString(std::to_string(s_2 + amount));
        score_2.setPosition(width - score_2.getLocalBounds().width, label_1.getLocalBounds().height + 5);
    }

    if(s_1 + s_2 > std::stoi(score_hi.getString().toAnsiString()))
    {
        score_hi.setString(std::to_string(s_1 + s_2));
    }
}

void scoreboard::draw(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    states.transform *= getTransform();

    target.draw(label_1, states);
    target.draw(label_hi, states);
    target.draw(label_2, states);
    target.draw(score_1, states);
    target.draw(score_hi, states);
    target.draw(score_2, states);
}
