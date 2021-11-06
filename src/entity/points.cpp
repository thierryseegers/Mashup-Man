#include "points.h"

#include "resources.h"

#include <SFML/Graphics.hpp>

#include <utility>

namespace entity
{

// Make a sprite out of the given number.
sprite make_sprite(
    unsigned int number)
{
    // Because RenderTexture must outlive the sprites that refer to them (or rather, to their Textures), this is static.
    static std::map<unsigned int, sf::RenderTexture> number_textures;

    // If a RenderTexture has not yet been created for that specific number, make one.
    decltype(number_textures)::const_iterator ci;
    if(ci = number_textures.find(number); ci == number_textures.end())
    {
        sf::Text t{std::to_string(number), resources::fonts().get(resources::font::retro), 16};
        auto const& local_bounds = t.getLocalBounds();

        sf::RenderTexture& rt = number_textures[number];
        rt.create(local_bounds.left + local_bounds.width, local_bounds.top + local_bounds.height);
        rt.clear(sf::Color::Transparent);
        rt.draw(t);
        rt.display();

        ci = number_textures.find(number);
    }

    return {ci->second.getTexture(), {0, 0, static_cast<int>(ci->second.getSize().x), static_cast<int>(ci->second.getSize().y)}};
}

points::points(
    unsigned int const number)
    : entity{
        make_sprite(number),
        75,
        direction::up
    }
    , duration_{sf::seconds(2.f)}
{
    throttle(1.f);

    // Because it's "headed" up, 'entity' will rotate it so we need to "counter-rotate" it.
    setRotation(90.f);
}

void points::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if((duration_ -= dt) <= sf::Time::Zero)
    {
        remove = true;
    }

    entity::update_self(dt, commands);
}

}
