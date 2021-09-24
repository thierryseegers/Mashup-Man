#include "entity/entity.h"

#include <magic_enum.hpp>
namespace entity
{

entity::entity(
    sprite sprite_,
    int const max_speed,
    direction const heading_)
    : sprite_{sprite_}
    , heading_{heading_}
    , max_speed{max_speed}
    , throttle_{0.f}
{
    head(heading_);
}

sf::FloatRect entity::collision_bounds() const
{
    return world_transform().transformRect(sprite_.global_bounds());
}

void entity::hit()
{
    remove = true;
}

direction entity::heading() const
{
    return heading_;
}

void entity::head(
    direction const d)
{
    switch(d)
    {
        case direction::up:
            sprite_.unflip();
            sprite_.set_rotation(270.f);
            break;
        case direction::down:
            sprite_.unflip();
            sprite_.set_rotation(90.f);
            break;
        case direction::left:
            sprite_.flip();
            sprite_.set_rotation(0);
            break;
        case direction::right:
            sprite_.unflip();
            sprite_.set_rotation(0);
            break;
        default:
            break;
    }

    heading_ = d;
}

float entity::speed() const
{
    return throttle_ * max_speed;
}

void entity::throttle(
    float const t)
{
    if((t == 0.f && throttle_ > 0.f) ||
       (t > 0.f && throttle_ == 0.f))
    {
        throttle_ = t;

        update_sprite();
    }
    else if(t != throttle_)
    {
        throttle_ = t;
    }
}

void entity::nudge(
    float const distance)
{
    nudge(heading(), distance);
}

void entity::nudge(
    direction const d,
    float const distance)
{
    auto const position = getPosition();

    setPosition(position.x + (d == direction::right ? distance : d == direction::left ? -distance : 0.f),
                position.y + (d == direction::down ? distance : d == direction::up ? -distance : 0.f));
}

void entity::update_sprite()
{}

void entity::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    static std::array<sf::Vector2f, magic_enum::enum_count<direction>()> const directions = []
    {
        return std::array<sf::Vector2f, magic_enum::enum_count<direction>()>{
                sf::Vector2f{0.f, -1.f},
                sf::Vector2f{0.f, 1.f},
                sf::Vector2f{-1.f, 0.f},
                sf::Vector2f{1.f, 0.f},
                sf::Vector2f{0.f, 0.f}};
    }();

    sf::Transformable::move(directions[magic_enum::enum_integer(heading_)] * (max_speed * throttle_) * dt.asSeconds());

    sprite_.update(dt, commands);
}

 void entity::draw_self(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    sprite_.draw(target, states);
}

}
