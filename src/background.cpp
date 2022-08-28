#include "background.h"

#include "resources.h"
#include "scene.h"
#include "sprite.h"

#include <SFML/Graphics.hpp>

background::background()
    : sprite_{resources::get(resources::texture::background)}
{
    sprite_.setColor({255, 255, 255, 255 / 6});
}

void background::draw_self(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    target.draw(sprite_, states);
}
