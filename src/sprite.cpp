#include "sprite.h"

#include "command.h"
#include "resources.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <spdlog/spdlog.h>

sprite::sprite(
    resources::texture const& texture,
    float const scale_factor)
    : sprite_{resources::textures().get(texture)}
    , scale_factor{scale_factor}
    , flipped{false}
    , current_frame{0}
    , elapsed{sf::Time::Zero}
{
    sprite_.setScale(scale_factor, scale_factor);
}

sprite::sprite(
    resources::texture const& texture,
    sf::IntRect const& texture_rect,
    float const scale_factor)
    : sprite(texture, scale_factor)
{
    still(texture_rect);
}

sprite::sprite(
    resources::texture const& texture,
    std::vector<sf::IntRect> const& texture_rects,
    sf::Time const duration,
    repeat const repeat_,
    float const scale_factor)
    : sprite(texture, scale_factor)
{
    animate(texture_rects, duration, repeat_);
}

void sprite::still(
    sf::IntRect const& texture_rect)
{
    sprite_.setTextureRect(texture_rect);
    utility::center_origin(sprite_);

    updater = [](sf::Time const&, commands_t&, sprite&){};
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
                commands_t&,
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
        sprite_.setScale(-scale_factor, scale_factor);
        flipped = true;
    }
}

void sprite::unflip()
{
    if(flipped)
    {
        sprite_.setScale(scale_factor, scale_factor);
        flipped = false;
    }
}

void sprite::set_rotation(
    float const angle)
{
    sprite_.setRotation(angle);
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
    target.draw(sprite_, states);
}

void sprite::update(
        sf::Time const& dt,
        commands_t& commands)
{
    updater(dt, commands, *this);
}
