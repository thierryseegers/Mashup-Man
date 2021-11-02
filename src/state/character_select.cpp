#include "character_select.h"

#include "entity/super_mario/hero.h"
#include "resources.h"
#include "state/stack.h"
#include "sprite.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <memory>
#include <vector>

namespace state
{

character_select::character_select(
    stack& states)
    : state{states}
{
    auto outline = sf::RectangleShape{{100.f, 100.f}};
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineColor({128, 128, 128});
    outline.setOutlineThickness(5.f);

    std::unique_ptr<entity::hero> ph = std::make_unique<entity::super_mario::mario<entity::hero_1>>();
    auto ds = ph->default_still();
    characters.push_back({std::move(ph), outline, ds});

    ph = std::make_unique<entity::super_mario::luigi<entity::hero_1>>();
    ds = ph->default_still();
    characters.push_back({std::move(ph), outline, ds});

    sprite unknown{resources::texture::items, {{0, 80, 16, 16}, {16, 80, 16, 16}, {32, 80, 16, 16}, {48, 80, 16, 16}}, sf::seconds(0.5f)};
    unknown.set_color({128, 128, 128});
    characters.push_back({nullptr, outline, unknown});
    // std::copy_n(characters.rend(), 5, std::back_inserter(characters));
    characters.push_back({nullptr, outline, unknown});
    characters.push_back({nullptr, outline, unknown});
    characters.push_back({nullptr, outline, unknown});
    characters.push_back({nullptr, outline, unknown});
    characters.push_back({nullptr, outline, unknown});

    float const x_offset = states.context.window.getSize().x / 2.f - characters.size() / 2.f * 100.f;
    float const y_offset = states.context.window.getSize().y / 2.f;
    for(std::size_t i = 0; i != characters.size(); ++i)
    {
        characters[i].outline.setPosition({x_offset + (outline.getSize().x + 2 * outline.getOutlineThickness()) * i, y_offset});
        characters[i].small.setPosition(characters[i].outline.getPosition() + sf::Vector2f{50.f, 50.f});
        characters[i].small.setScale(100.f / 16, 100.f / 16);
    }

    selection s{0, sf::RectangleShape{outline.getSize() - 2.f * sf::Vector2f{outline.getOutlineThickness(), outline.getOutlineThickness()}}, characters[0].hero->default_animated(), false};
    s.outline.setFillColor(sf::Color::Transparent);
    s.outline.setOutlineColor(sf::Color::Green);
    s.outline.setOutlineThickness(outline.getOutlineThickness());
    s.outline.setPosition(characters[0].outline.getPosition() + sf::Vector2f{outline.getOutlineThickness(), outline.getOutlineThickness()});
    
    s.big.setPosition(states.context.window.getSize().x / 4.f, states.context.window.getSize().x / 4.f);
    s.big.setScale(states.context.window.getSize().y / 4.f / 16.f, states.context.window.getSize().y / 4.f / 16.f);

    selections.push_back(s);

    if(states.context.players.size() >= 2)
    {
        s.n = 1;

        s.outline.setOutlineColor(sf::Color::Red);
        s.outline.move({characters[0].outline.getSize().x + 2 * outline.getOutlineThickness(), 0.f});

        s.big = characters[1].hero->default_animated();
        s.big.setPosition(selections[0].big.getPosition() + sf::Vector2f{states.context.window.getSize().x / 2.f, 0.f});
        s.big.setScale(selections[0].big.getScale());
        s.big.flip();

        selections.push_back(s);
    }
}

void character_select::draw()
{
    auto& window = states.context.window;

    for(auto const& c : characters)
    {
        window.draw(c.outline);
        window.draw(c.small);
    }

    for(auto const& s : selections)
    {
        window.draw(s.outline);
        window.draw(s.big);
    }
}

bool character_select::update(
    sf::Time const& dt)
{
    for(auto& c : characters)
    {
        c.small.update(dt);
    }

    for(auto& s : selections)
    {
        s.big.update(dt);
    }

    return true;
}

bool character_select::handle_event(
    sf::Event const& event)
{
    // if((event.type == sf::Event::KeyReleased &&
    //     event.key.code == sf::Keyboard::Enter) ||
    //    (event.type == sf::Event::JoystickButtonReleased && 
    //     event.joystickButton.button == 6))
    if(event.type == sf::Event::JoystickButtonReleased && 
       event.joystickButton.button == 6)
    {
        states.context.players.clear(); // title state remains on the state stack. Clear the players before adding new ones.

        states.context.players.push_back(std::make_unique<player>(std::type_identity<entity::super_mario::mario<entity::hero_1>>{}));
    //     if(num_players >= 2)
    //     {
    //         states.context.players.push_back(std::make_unique<player>(std::type_identity<entity::super_mario::luigi<entity::hero_2>>{}));
    //     }

    //     states.context.window.setView(sf::View{sf::FloatRect{0, 0, (float)states.context.window.getSize().x, (float)states.context.window.getSize().y}});

        states.request_pop();
        states.request_push(id::game);
    }
    // else if((event.type == sf::Event::KeyReleased &&
    //          (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)) ||
    //         (event.type == sf::Event::JoystickButtonReleased && 
    //          event.joystickButton.button == 4))
    // {
    //     num_players = num_players % 2 + 1;

    //     arrow.setPosition(arrow.getPosition().x, choices.getGlobalBounds().top - 25 + (num_players - 1) * 100);
    // }

    return true;
}

}
