#include "state/title.h"

#include "resources.h"
#include "state/stack.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace state
{

title::title(
    stack& states)
    : state{states}
    , title_{"Mashup-Man", resources::fonts().get(resources::font::main), 150}
    , choices{"1 PLAYER\n2 PLAYERS", resources::fonts().get(resources::font::retro), 75}
    , arrow{">", resources::fonts().get(resources::font::retro), 75}
    , num_players{1}
{
    utility::center_origin(title_, choices);

    title_.setFillColor(sf::Color::Yellow);

    auto const view_size = states.context.window.getView().getSize();
    title_.setPosition(view_size.x * 0.5f, view_size.y * 0.3f);
    choices.setPosition(view_size.x * 0.5f, title_.getGlobalBounds().top + title_.getGlobalBounds().height + choices.getLocalBounds().height / 2 + 100.f);
    arrow.setPosition(choices.getGlobalBounds().left - 2 * arrow.getFont()->getGlyph('>', 75, false).bounds.width, choices.getGlobalBounds().top - 25);
}

void title::draw()
{
    auto& window = states.context.window;

    window.draw(title_);
    window.draw(choices);
    window.draw(arrow);
}

bool title::update(
    sf::Time const& /*dt*/)
{
    return true;
}

bool title::handle_event(
    sf::Event const& event)
{
    if((event.type == sf::Event::KeyReleased &&
        event.key.code == sf::Keyboard::Enter) ||
       (event.type == sf::Event::JoystickButtonReleased && 
        event.joystickButton.button == 6))
    {
        states.request_pop();
        states.request_push(id::game);
    }
    else if((event.type == sf::Event::KeyReleased &&
             (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)) ||
            (event.type == sf::Event::JoystickButtonReleased && 
             event.joystickButton.button == 5))
    {
        num_players = num_players % 2 + 1;

        arrow.setPosition(arrow.getPosition().x, choices.getGlobalBounds().top - 25 + (num_players - 1) * 100);
    }

    return true;
}

}
