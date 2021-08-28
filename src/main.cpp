#include "configuration.h"
#include "game.h"

#include <iostream>
#include <stdexcept>

int main(int, char**)
{
    try
    {
        configuration::initialize("./config.toml");

        std::cout << configuration::values();
        
        game{}.run();
    }
    catch(std::exception const& e)
    {
        std::cerr << "[exception] " << e.what() << std::endl;
    }

    return 0;
}