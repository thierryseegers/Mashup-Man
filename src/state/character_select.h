#pragma once

#include "entity/hero.h"
#include "sprite.h"
#include "state/stack.h"
#include "state/state.h"
#include "utility.h"

#include <SFML/Graphics/RoundedRectangleShape.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <memory>
#include <vector>

namespace state
{

class character_select
    : public state
{
public:
    character_select(
        stack& states);

    virtual void draw() override;

    virtual bool update(
        sf::Time const& dt) override;

    virtual bool handle_event(
        sf::Event const& event) override;

private:
    struct character
    {
        std::unique_ptr<entity::hero> hero;
        sf::RectangleShape outline;
        sprite small;
    };
    std::vector<character> characters;

    struct selection
    {
        utility::cyclic_iterator<decltype(characters)> ci;
        sf::RectangleShape outline;
        sprite big;
        bool selected;
    };
    std::vector<selection> selections;
};

}
