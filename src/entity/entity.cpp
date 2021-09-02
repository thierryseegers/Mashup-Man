#include "entity/entity.h"

namespace entity
{

entity::entity(
    sprite sprite_)
    : velocity{}
    , sprite_{sprite_}
{}

sf::FloatRect entity::collision_bounds() const
{
    return world_transform().transformRect(sprite_.getGlobalBounds());
}

void entity::play_local_sound(
    commands_t& commands,
    resources::sound_effect const se) const
{
    commands.push(make_command<scene::sound_t>([=](scene::sound_t& s, sf::Time const&)
    {
        s.play(se, world_position());
    }));
}

void entity::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    sf::Transformable::move(velocity * dt.asSeconds());

    sprite_.update(dt, commands);
}

 void entity::draw_self(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    sprite_.draw(target, states);
}

}
