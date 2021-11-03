#include "lifeboard.h"

#include "level.h"

size_t lifeboard::grant(
    int const lives,
    sprite const& sprite)
{
    infos.emplace_back(lives, sprite);

    return infos.size() - 1;
}

int lifeboard::take(
        size_t const i)
{
    return --infos[i].lives;
}

void lifeboard::draw(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    states.transform *= getTransform();

    for(int i = 0; i != infos[0].lives; ++i)
    {
        infos[0].sprite_.setPosition(i * (level::tile_size + level::half_tile_size), 0);
        target.draw(infos[0].sprite_, states);
    }

    if(infos.size() >= 2)
    {
        for(int i = 0; i != infos[1].lives; ++i)
        {
            infos[1].sprite_.setPosition(level::width * level::tile_size -  i * (level::tile_size + level::half_tile_size), 0);
            target.draw(infos[1].sprite_, states);
        }
    }
}
