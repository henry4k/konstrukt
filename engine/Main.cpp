#include "Common.h"
#include "Game.h"

int main( const int argc, char** argv )
{
    if(!InitGame(argc, argv))
        return EXIT_FAILURE;
    RunGame();
    DestroyGame();
    return EXIT_SUCCESS;
}
