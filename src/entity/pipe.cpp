#include "entity/pipe.h"

#include "configuration.h"
#include "entity.h"
#include "resources.h"
#include "sprite.h"

namespace entity
{

pipe::pipe()
    : entity{
        sprite{
            resources::texture::pipe,
            sf::IntRect{0, 0, 32, 32}}}
{
    auto const scale = configuration::value_or<float>(1.f)["features"]["pipe"]["scale"];
    setScale(scale, scale);
}

}