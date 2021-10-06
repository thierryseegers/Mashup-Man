#include "entity/character.h"

#include "resources.h"
#include "scene.h"
#include "sprite.h"
#include "utility.h"

#include <SFML/System.hpp>

#include <memory>

namespace entity
{

void character::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    entity::update_self(dt, commands);
}

}
