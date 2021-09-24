#include "state/title.h"

#include "entity/brothers.h"
#include "resources.h"
#include "state/stack.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <algorithm>

namespace state
{

bool title::scroll = true;

title::title(
    stack& states)
    : state{states}
    , view{states.context.window.getView()}
    , title_{"Mashup-Man", resources::fonts().get(resources::font::main), 150}
    , frame{{title_.getLocalBounds().width + 100, title_.getLocalBounds().height + 40}, 20, 8}
    , outer_frame{{frame.getSize().x + 40, frame.getSize().y + 40}, 28, 12}
    , choices{"1 PLAYER\n2 PLAYERS", resources::fonts().get(resources::font::retro), 75}
    , arrow{">", resources::fonts().get(resources::font::retro), 75}
    , num_players{1}
{
    utility::center_origin(title_, frame, outer_frame, choices);

    frame.setFillColor(sf::Color::Blue);
    frame.setOutlineColor(sf::Color::Black);
    frame.setOutlineThickness(5.f);
    outer_frame.setFillColor(sf::Color::Magenta);

    title_.setFillColor(sf::Color::Yellow);

    auto const view_size = states.context.window.getView().getSize();
    title_.setPosition(view_size.x * 0.5f, view_size.y * 0.3f);
    frame.setPosition(title_.getPosition());
    outer_frame.setPosition(frame.getPosition());

    choices.setPosition(view_size.x * 0.5f, title_.getGlobalBounds().top + title_.getGlobalBounds().height + choices.getLocalBounds().height / 2 + 200.f);
    arrow.setPosition(choices.getGlobalBounds().left - 2 * arrow.getFont()->getGlyph('>', 75, false).bounds.width, choices.getGlobalBounds().top - 25);

    if(scroll)
    {
        scroll = false;

        to_scroll = view_size.y;
        view.move(0, -to_scroll);
    }
    else
    {
        to_scroll = 0;
    }
}

void title::draw()
{
    auto& window = states.context.window;
    window.setView(view);

    window.draw(outer_frame);
    window.draw(frame);
    window.draw(title_);
    window.draw(choices);
    window.draw(arrow);
}

bool title::update(
    sf::Time const&)
{
    if(to_scroll -= std::min(to_scroll, 20.f); to_scroll > 0.f)
    {
        view.move(0, 20.f);
    }

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
        states.context.players.push_back(std::make_unique<player>(0, []{ return std::make_unique<entity::mario>(); }));
        if(num_players >= 2)
        {
            states.context.players.push_back(std::make_unique<player>(1, []{ return std::make_unique<entity::luigi>(); }));
        }

        states.context.window.setView(sf::View{sf::FloatRect{0, 0, (float)states.context.window.getSize().x, (float)states.context.window.getSize().y}});

        states.request_pop();
        states.request_push(id::game);
    }
    else if((event.type == sf::Event::KeyReleased &&
             (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)) ||
            (event.type == sf::Event::JoystickButtonReleased && 
             event.joystickButton.button == 4))
    {
        num_players = num_players % 2 + 1;

        arrow.setPosition(arrow.getPosition().x, choices.getGlobalBounds().top - 25 + (num_players - 1) * 100);
    }

    return true;
}

}
