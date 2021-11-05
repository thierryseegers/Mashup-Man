#pragma once

#include "entity/entity.h"

namespace entity
{

class projectile
    : public entity
{
public:
    projectile(
        unsigned int const player_id_,
        sprite sprite_,
        int const max_speed = 0,
        direction const heading_ = direction::right);

    virtual ~projectile() = default;

    unsigned int player_id() const;

private:
    unsigned int player_id_;
};

}
