#include "Game.h"

void Game::pollEvents(EventHandle& hndl)
{
	if (!hndl.getAllEventofType(Events::ESCAPE).empty()) { running = false; }
}
