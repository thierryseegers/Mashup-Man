#include "lifeboard.h"

#include "level.h"

lifeboard::info& lifeboard::operator[](
    size_t const i)
{
    if(i >= infos.size())
    {
        infos.resize(i + 1);
    }

    return infos[i];
}

void lifeboard::draw(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    states.transform *= getTransform();
    
    for(int i = 0; i != infos[0].lives; ++i)
    {
        infos[0].sprite.setPosition(i * (level::tile_size + level::half_tile_size), 0);
        target.draw(infos[0].sprite, states);
    }

    if(infos.size() >= 2)
    {
        for(int i = 0; i != infos[1].lives; ++i)
        {
            infos[1].sprite.setPosition(level::width * level::tile_size -  i * (level::tile_size + level::half_tile_size), 0);
            target.draw(infos[1].sprite, states);
        }
    }
}
