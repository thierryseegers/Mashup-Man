#include "lifeboard.h"

#include "level.h"

lifeboard::lifeboard(
    sf::Sprite sprite_1,
    sf::Sprite sprite_2)
    : player_1{sprite_1, 0}
    , player_2{sprite_2, 0}
{
    // auto const sprite_2_rect = player_2.sprite.getTextureRect();
    // player_2.sprite.setTextureRect(sf::IntRect{sprite_2_rect.width, 0, -sprite_2_rect.width, sprite_2_rect.height});
    player_2.sprite.setScale(-1, 1);
}

int& lifeboard::operator[](
    int const i)
{
    if(i == 0)
    {
        return player_1.lives;
    }
    else
    {
        return player_2.lives;
    }
}

void lifeboard::draw(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    states.transform *= getTransform();
    
    for(int i = 0; i != player_1.lives; ++i)
    {
        player_1.sprite.setPosition(i * (level::tile_size + level::half_tile_size), 0);
        target.draw(player_1.sprite, states);
    }

    for(int i = 0; i != player_2.lives; ++i)
    {
        player_2.sprite.setPosition(level::width * level::tile_size -  i * (level::tile_size + level::half_tile_size), 0);
        target.draw(player_2.sprite, states);
    }
}