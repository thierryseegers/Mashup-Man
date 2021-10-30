#include "sprite.h"

#include "command.h"
#include "resources.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <spdlog/spdlog.h>

sprite::sprite(
    resources::texture const& texture)
    : sprite_{resources::textures().get(texture)}
    , flipped{false}
    , current_frame{0}
    , elapsed{sf::Time::Zero}
{}

sprite::sprite(
    resources::texture const& texture,
    sf::IntRect const& texture_rect)
    : sprite(texture)
{
    still(texture_rect);
}

sprite::sprite(
    resources::texture const& texture,
    std::vector<sf::IntRect> const& texture_rects,
    sf::Time const duration,
    repeat const repeat_)
    : sprite(texture)
{
    animate(texture_rects, duration, repeat_);
}

void sprite::still(
    sf::IntRect const& texture_rect)
{
    sprite_.setTextureRect(texture_rect);
    utility::center_origin(sprite_);

    updater = [](sf::Time const&, sprite&){};
}

void sprite::animate(
    std::vector<sf::IntRect> const& texture_rects,
    sf::Time const duration,
    repeat const repeat_)
{
    sprite_.setTextureRect(texture_rects[0]);
    utility::center_origin(sprite_);

    updater = [=,
               n_frames = texture_rects.size(),
               time_per_frame = duration / static_cast<float>(texture_rects.size())](
                sf::Time const& dt,
                sprite& sprite_) mutable
    {
        sprite_.elapsed += dt;

        while(sprite_.elapsed >= time_per_frame && (sprite_.current_frame <= n_frames || repeat_ != repeat::none))
        {
            sprite_.elapsed -= time_per_frame;

            if(repeat_ == repeat::loop)
            {
                sprite_.current_frame = (sprite_.current_frame + 1) % n_frames;
            }
            else
            {
                ++sprite_.current_frame;
            }
        }

        sprite_.sprite_.setTextureRect(texture_rects[sprite_.current_frame]);
    };
}

void sprite::flip()
{
    if(!flipped)
    {
        auto const scale = sprite_.getScale();
        sprite_.setScale(-scale.x, scale.y);
        flipped = true;
    }
}

void sprite::unflip()
{
    if(flipped)
    {
        auto const scale = sprite_.getScale();
        sprite_.setScale(-scale.x, scale.y);
        flipped = false;
    }
}

void sprite::set_color(
    sf::Color const color)
{
    sprite_.setColor(color);
}

sf::FloatRect sprite::global_bounds() const
{
    return sprite_.getGlobalBounds();
}

void sprite::draw(
    sf::RenderTarget& target,
    sf::RenderStates states) const 
{
    states.transform *= getTransform();
    target.draw(sprite_, states);
}

void sprite::update(
        sf::Time const& dt)
{
    updater(dt, *this);
}
