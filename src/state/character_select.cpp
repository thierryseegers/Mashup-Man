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
    , elapsed{sf::Time::Zero}
{
    // Set up 'characters'.
    auto outline = sf::RectangleShape{{100.f, 100.f}};
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineColor({128, 128, 128});
    outline.setOutlineThickness(5.f);

    std::unique_ptr<entity::hero> ph = std::make_unique<entity::super_mario::mario>(99);
    auto ds = ph->default_still();
    characters.push_back({std::move(ph), outline, ds});

    ph = std::make_unique<entity::super_mario::luigi>(99);
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
        characters[i].small_.setPosition(characters[i].outline.getPosition() + sf::Vector2f{50.f, 50.f});
        characters[i].small_.setScale(100.f / 16, 100.f / 16);
    }

    // Set up 'selections'.
    selection s{{characters, characters.begin()}, sf::RectangleShape{outline.getSize() - 2.f * sf::Vector2f{outline.getOutlineThickness(), outline.getOutlineThickness()}}, sf::Color::Red, characters[0].hero->default_animated(), false};
    s.outline.setFillColor(sf::Color::Transparent);
    s.outline.setOutlineThickness(outline.getOutlineThickness());
    s.outline.setPosition(s.ci->outline.getPosition() + sf::Vector2f{outline.getOutlineThickness(), outline.getOutlineThickness()});

    s.big.setPosition(states.context.window.getSize().x / 4.f, states.context.window.getSize().x / 4.f);
    s.big.setScale(states.context.window.getSize().y / 4.f / 16.f, states.context.window.getSize().y / 4.f / 16.f);

    selections.push_back(s);

    if(states.context.players.size() >= 2)
    {
        ++s.ci;

        s.outline.setPosition(s.ci->outline.getPosition() + sf::Vector2f{outline.getOutlineThickness(), outline.getOutlineThickness()});
        s.color = sf::Color::Green;

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
        window.draw(c.small_);
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
    elapsed += dt;

    for(auto& c : characters)
    {
        c.small_.update(dt);
    }

    for(auto& s : selections)
    {
        s.big.update(dt);
        if(s.selected)
        {
            s.outline.setOutlineColor(s.color);
        }
        else
        {
            // To have the color pulse between black and `s.color` over a period of half a second, we use a triangle wave function (c.f. https://en.wikipedia.org/wiki/Triangle_wave).
            s.outline.setOutlineColor(s.color * sf::Color{255, 255, 255, static_cast<sf::Uint8>(255 * 2 * fabs(elapsed.asSeconds() / .5f - floorf(elapsed.asSeconds() / .5f + .5f)))});
        }
    }

    return true;
}

bool character_select::handle_event(
    sf::Event const& event)
{
    // 'Joystick Left' or 'Joystick Right'
    if(event.type == sf::Event::JoystickMoved &&
       event.joystickMove.joystickId < selections.size() &&
       !selections[event.joystickMove.joystickId].selected)
    {
        auto& s = selections[event.joystickMove.joystickId];
        if(auto const x_axis = sf::Joystick::getAxisPosition(event.joystickMove.joystickId, sf::Joystick::Axis::X); x_axis > 1)
        {
            // Find the next unoccupied selection.
            do
            {
                ++s.ci;
            }
            while(std::count_if(selections.begin(), selections.end(), [&](auto const& ss){ return ss.ci == s.ci; }) > 1);
        }
        else if(x_axis < -1)
        {
            // Find the preceding unoccupied selection.
            do
            {
                --s.ci;
            }
            while(std::count_if(selections.begin(), selections.end(), [&](auto const& ss){ return ss.ci == s.ci; }) > 1);
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
            if(event.joystickMove.joystickId > 0)
            {
                s.big.flip();
            }
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
            event.joystickMove.joystickId < selections.size() &&
            event.joystickButton.button == 4 &&
            selections[event.joystickButton.joystickId].ci->hero)
    {
        auto& s = selections[event.joystickButton.joystickId];
        s.selected = !s.selected;
        if(s.selected)
        {
            states.context.sound.play(resources::sound_effect::collect_powerup);
        }
    }
    // 'Enter' key or 'Start' button.
    if((event.type == sf::Event::KeyReleased &&
        event.key.code == sf::Keyboard::Enter) ||
       (event.type == sf::Event::JoystickButtonReleased &&
        event.joystickMove.joystickId == 0 &&
        event.joystickButton.button == 6))
    {
        if(std::all_of(selections.begin(), selections.end(), [](auto const& s){ return s.selected; }))
        {
            states.context.players.clear();

            for(unsigned int i = 0; i != selections.size(); ++i)
            {
                // I'm absolutely aware of what this looks like...!
                if(dynamic_cast<entity::super_mario::mario*>(selections[i].ci->hero.get()))
                {
                    states.context.players.push_back(std::make_unique<player>(i, std::type_identity<entity::super_mario::mario>{}));
                }
                else if(dynamic_cast<entity::super_mario::luigi*>(selections[i].ci->hero.get()))
                {
                    states.context.players.push_back(std::make_unique<player>(i, std::type_identity<entity::super_mario::luigi>{}));
                }
            }

            states.request_pop();
            states.request_push(id::game);
        }
    }

    return true;
}

}
