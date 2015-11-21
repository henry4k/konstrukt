#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include "Game.h"

int main( const int argc, char** argv )
{
    if(!InitGame(argc, argv))
        return EXIT_FAILURE;
    RunGame();
    return EXIT_SUCCESS;
}

