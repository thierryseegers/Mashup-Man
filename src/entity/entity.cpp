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
    heading_ = d;

    update_sprite();
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

void entity::update_self(
    sf::Time const& dt,
    commands_t&)
{
    sf::Transformable::move(to_vector2f(heading_) * (max_speed * throttle_) * dt.asSeconds());

    sprite_.update(dt);
}

 void entity::draw_self(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    sprite_.draw(target, states);
}

void entity::update_sprite()
{
    switch(heading_)
    {
        case direction::up:
            sprite_.unflip();
            sprite_.setRotation(270.f);
            break;
        case direction::down:
            sprite_.unflip();
            sprite_.setRotation(90.f);
            break;
        case direction::left:
            sprite_.flip();
            sprite_.setRotation(0.f);
            break;
        case direction::right:
            sprite_.unflip();
            sprite_.setRotation(0.f);
            break;
        default:
            break;
    }
}

}
