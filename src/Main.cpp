#include "Common.h"
#include "Game.h"

int main()
{
	if(!InitGame())
		return EXIT_FAILURE;
	RunGame();
	FreeGame();
    return EXIT_SUCCESS;
}
