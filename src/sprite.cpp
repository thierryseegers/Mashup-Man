#include "sprite.h"

#include "command.h"
#include "resources.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

sprite_t::sprite_t(
    resources::texture const& texture,
    sf::IntRect const& texture_rect,
    float const scale)
    : sprite{resources::textures().get(texture), texture_rect}
{
    utility::center_origin(sprite);
    sprite.setScale(scale, scale);
}

sf::FloatRect sprite_t::getGlobalBounds() const
{
    return sprite.getGlobalBounds();
}

void sprite_t::draw(
    sf::RenderTarget& target,
    sf::RenderStates states) const 
{
    target.draw(sprite, states);
}

animated_sprite_t::animated_sprite_t(
    resources::texture const& texture_sheet,
    std::vector<sf::IntRect> const& texture_rects,
    sf::Time const duration,
    repeat const repeat_,
    float const scale)
    : sprite_t{texture_sheet, texture_rects[0], scale}
    , texture_rects{texture_rects}
    , n_frames{texture_rects.size()}
    , current_frame{0}
    , duration{duration}
    , elapsed{sf::Time::Zero}
    , repeat_{repeat_}
{}

void animated_sprite_t::update(
    sf::Time const& dt,
    commands_t&)
{
    auto const time_per_frame{duration / static_cast<float>(n_frames)};
    elapsed += dt;

    // auto texture_rect{sprite.getTextureRect()};

    // if(current_frame == 0)
    // {
    //     texture_rect = texture_rects[0];
    // }

    while(elapsed >= time_per_frame && (current_frame <= n_frames || repeat_ != repeat::none))
    {
        // // Move texture rect to next rect.
        // texture_rect.left += frame_size.x;
        // if(texture_rect.left + texture_rect.width > bounds.width)
        // {
        //     texture_rect.left = 0;
        //     texture_rect.top += frame_size.y;
        // }

        elapsed -= time_per_frame;

        if(repeat_ != repeat::none)
        {
            current_frame = (current_frame + 1) % n_frames;
            // if(current_frame == 0)
            // {
            //     texture_rect = {bounds.left, bounds.top, frame_size.x, frame_size.y};
            // }
        }
        else
        {
            ++current_frame;
        }
    }

    sprite.setTextureRect(texture_rects[current_frame]);
}
