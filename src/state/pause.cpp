#include "state/pause.h"

#include "resources.h"
#include "services/services.h"
#include "state/stack.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace state
{

pause::pause(
    stack& states)
    : state{states}
    , word{"Game Paused", resources::get(resources::font::main), 100}
    , instructions{"(Press Backspace or L + R to quit)", resources::get(resources::font::retro)}
{
    utility::center_origin(word, instructions);

    auto const view_size = states.context.window.getView().getSize();
    word.setPosition(view_size / 2.f);
    instructions.setPosition(view_size.x / 2, word.getPosition().y + word.getCharacterSize());

    services::sound_player::value().pause();
    services::sound_player::value().play(resources::sound_effect::pause);
}

pause::~pause()
{
    services::sound_player::value().resume();
    services::sound_player::value().play(resources::sound_effect::pause);
}

void pause::draw()
{
    auto& window = states.context.window;

    sf::RectangleShape foreground;
    foreground.setFillColor({0, 0, 0, 150});
    foreground.setSize(sf::Vector2f{window.getView().getSize()});

    window.draw(foreground);
    window.draw(word);
    window.draw(instructions);
}

bool pause::update(
    sf::Time const&)
{
    return false;
}

bool pause::handle_event(
    sf::Event const& event)
{
    if(event.type == sf::Event::Resized)
    {
        auto const view_size = states.context.window.getView().getSize();

        word.setPosition(view_size.x / 2, view_size.y / 2);
        instructions.setPosition(view_size.x / 2, word.getPosition().y + word.getCharacterSize());

        return true;
    }

    // Escape key or Start button pressed, remove itself to return to the game.
    if(event.type == sf::Event::KeyReleased &&
       event.key.code == sf::Keyboard::Escape)
    {
        states.request_pop();
    }
    else if(event.type == sf::Event::JoystickButtonReleased &&
            event.joystickButton.button == 6)
    {
        states.request_pop();
    }

    // Backspace key pressed or Left and Right button pressed, clear stack and push main menu.
    if(event.type == sf::Event::KeyReleased &&
       event.key.code == sf::Keyboard::BackSpace)
    {
        states.request_clear();
        states.request_push(id::title);
    }
    else if(event.type == sf::Event::JoystickButtonPressed)
    {
        if((event.joystickButton.button == 5 && sf::Joystick::isButtonPressed(0, 7)) ||
           (event.joystickButton.button == 7 && sf::Joystick::isButtonPressed(0, 5)))
        {
            states.request_clear();
            states.request_push(id::title);
        }
    }

    return false;
}

}
