#include "level_complete.h"

#include "state/stack.h"
#include "state/state.h"
// #include "player.h"
#include "resources.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace state
{

level_complete::level_complete(
    stack& states)
    : state{states}
    , text{"Level complete!", resources::fonts().get(resources::font::main), 100}
    , duration{resources::sound_buffers().get(resources::sound_effect::level_complete).getDuration()}
    , elapsed{sf::Time::Zero}
    , fireworks{1 + 2 * utility::random(1) + 2 * utility::random(1)}   // 1, 3 or 5 firework(s)
{
    utility::center_origin(text);

    auto const view_size = states.context.window.getView().getSize();
    text.setPosition(view_size.x / 2, view_size.y / 2);

    states.context.sound.play(resources::sound_effect::level_complete);
}

void level_complete::draw()
{
    sf::RenderWindow& window = states.context.window;

    // Create dark, semi-transparent background.
    sf::RectangleShape background;
    background.setFillColor(sf::Color(0, 0, 0, 150));
    background.setSize(window.getView().getSize());

    window.draw(background);
    window.draw(text);

    if(explosion)
    {
        window.draw(*explosion);
    }
}

bool level_complete::update(
    sf::Time const& dt)
{
    // Show state for a set time before returning to game screen.
    elapsed += dt;
    if(elapsed >= duration)
    {
        if(fireworks--)
        {
            elapsed = sf::Time::Zero;
            duration = resources::sound_buffers().get(resources::sound_effect::fireworks).getDuration();

            states.context.sound.play(resources::sound_effect::fireworks);

            explosion = std::make_unique<sprite>(resources::texture::items, std::vector<sf::IntRect>{{112, 144, 16, 16}, {112, 160, 16, 16}, {112, 176, 16, 16}}, resources::sound_buffers().get(resources::sound_effect::fireworks).getDuration(), sprite::repeat::none);
            auto const& p = text.getPosition();
            explosion->setPosition({p.x + utility::random(500) * (-1 + 2 * utility::random(1)), p.y + utility::random(500) * (-1 + 2 * utility::random(1))});
            explosion->setScale({12.f, 12.f});
        }
        else
        {
            states.request_clear();

            // TODO: Specify the next level in `states`.

            states.request_push(id::game);
        }
    }
    else if(explosion)
    {
        explosion->update(dt);
    }

    return false;
}

bool level_complete::handle_event(
    sf::Event const& event)
{
    if(event.type == sf::Event::Resized)
    {
        auto const view_size = states.context.window.getView().getSize();
        text.setPosition(view_size.x / 2, view_size.y / 2);

        return true;
    }

    return false;
}

}
