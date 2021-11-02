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

    selection s{{characters, characters.begin()}, sf::RectangleShape{outline.getSize() - 2.f * sf::Vector2f{outline.getOutlineThickness(), outline.getOutlineThickness()}}, characters[0].hero->default_animated(), false};
    s.outline.setFillColor(sf::Color::Transparent);
    s.outline.setOutlineColor(sf::Color::Green);
    s.outline.setOutlineThickness(outline.getOutlineThickness());
    s.outline.setPosition(s.ci->outline.getPosition() + sf::Vector2f{outline.getOutlineThickness(), outline.getOutlineThickness()});

    s.big.setPosition(states.context.window.getSize().x / 4.f, states.context.window.getSize().x / 4.f);
    s.big.setScale(states.context.window.getSize().y / 4.f / 16.f, states.context.window.getSize().y / 4.f / 16.f);

    selections.push_back(s);

    if(states.context.players.size() >= 2)
    {
        ++s.ci;

        s.outline.setOutlineColor(sf::Color::Red);
        s.outline.setPosition(s.ci->outline.getPosition() + sf::Vector2f{outline.getOutlineThickness(), outline.getOutlineThickness()});

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
    // 'Joystick Left' or 'Joystick Right'
    if(event.type == sf::Event::JoystickMoved &&
       !selections[event.joystickMove.joystickId].selected)
    {
        auto& s = selections[event.joystickMove.joystickId];
        if(auto const x_axis = sf::Joystick::getAxisPosition(event.joystickMove.joystickId, sf::Joystick::Axis::X); x_axis > 1)
        {
            s.ci += (selections.size() >= 2 && selections[1].ci == (s.ci + 1)) ? 2 : 1;
        }
        else if(x_axis < -1)
        {
            s.ci -= (selections.size() >= 2 && selections[1].ci == (s.ci - 1)) ? 2 : 1;
        }
        else
        {
            return true;
        }

        s.outline.setPosition(s.ci->outline.getPosition() + sf::Vector2f{s.ci->outline.getOutlineThickness(), s.ci->outline.getOutlineThickness()});
        if(s.ci->hero)
        {
            auto const position = s.big.getPosition();
            auto const scale = s.big.getScale();
            s.big = s.ci->hero->default_animated();
            s.big.setPosition(position);
            s.big.setScale(scale);
            s.big.set_color(sf::Color::White);
        }
        else
        {
            s.big.set_color(sf::Color::Black);
        }

        states.context.sound.play(resources::sound_effect::collect_coin);
    }
    // 'Select' button
    else if(event.type == sf::Event::JoystickButtonReleased &&
            event.joystickButton.button == 4 &&
            selections[event.joystickButton.joystickId].ci->hero)
    {
        auto& s = selections[event.joystickButton.joystickId];
        s.selected = !s.selected;
        if(s.selected)
        {
            s.outline.setOutlineColor(sf::Color::White);

            states.context.sound.play(resources::sound_effect::collect_powerup);
        }
        else
        {

        }
    }
    // 'Enter' key or 'Start' button.
    if((event.type == sf::Event::KeyReleased &&
        event.key.code == sf::Keyboard::Enter) ||
       (event.type == sf::Event::JoystickButtonReleased && 
        event.joystickButton.button == 6))
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

    return true;
}

}
