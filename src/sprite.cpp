#include "sprite.h"

#include "command.h"
#include "resources.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <spdlog/spdlog.h>

sprite::sprite(
    resources::texture const& texture,
    sf::IntRect const& texture_rect,
    float const scale_factor)
    : sprite_{resources::textures().get(texture)}
    , scale_factor{scale_factor}
    , flipped{false}
{
    still(texture_rect);

    sprite_.setScale(scale_factor, scale_factor);
}

sprite::sprite(
    resources::texture const& texture,
    std::vector<sf::IntRect> const& texture_rects,
    sf::Time const duration,
    repeat const repeat_,
    float const scale_factor)
    : sprite_{resources::textures().get(texture)}
    , scale_factor{scale_factor}
    , flipped{false}
{
    animate(texture_rects, duration, repeat_);

    sprite_.setScale(scale_factor, scale_factor);
}

void sprite::still(
    sf::IntRect const& texture_rect)
{
    sprite_.setTextureRect(texture_rect);
    utility::center_origin(sprite_);

    updater = [](sf::Time const&, commands_t&, sf::Sprite&){};
}

void sprite::animate(
    std::vector<sf::IntRect> const& texture_rects,
    sf::Time const duration,
    repeat const repeat_)
{
    sprite_.setTextureRect(texture_rects[0]);
    utility::center_origin(sprite_);

    // As expressed in the header file, this ought to just capture and manipulate 
    // 'this->sprite_' rather than be passed a reference to it. Clue: it actually 
    // works when calling 'animate' after the object is constructed but it doesn't 
    // when called from the constructor.
    updater = [=,
               current_frame = std::size_t{0},
               elapsed = sf::Time::Zero,
               n_frames = texture_rects.size(),
               time_per_frame = duration / static_cast<float>(texture_rects.size())]
               (
                sf::Time const& dt,
                commands_t&,
                sf::Sprite& sprite) mutable
    {
        elapsed += dt;

        while(elapsed >= time_per_frame && (current_frame <= n_frames || repeat_ != repeat::none))
        {
            elapsed -= time_per_frame;

            if(repeat_ == repeat::loop)
            {
                current_frame = (current_frame + 1) % n_frames;
            }
            else
            {
                ++current_frame;
            }
        }

        sprite.setTextureRect(texture_rects[current_frame]);
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

sf::FloatRect sprite::getGlobalBounds() const
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
    updater(dt, commands, sprite_);
}
