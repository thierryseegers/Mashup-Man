#include "entity/projectile.h"

namespace entity
{
projectile::projectile(
    unsigned int const player_id_,
    sprite sprite_,
    int const max_speed,
    direction const heading_)
    : entity{sprite_, max_speed, heading_}
    , player_id_{player_id_}
{}

unsigned int projectile::player_id() const
{
    return player_id_;
}

}
