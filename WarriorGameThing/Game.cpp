#include "Game.h"

void Game::pollEvents(EventHandle& hndl)
{
	if (!hndl.getAllEventofType(Events::ESCAPE).empty()) {
		hndl.listEvents();
		running = false;
	}

	eventPtr mouseMovement = hndl.getFirstofType(Events::MOUSE_MOVE);

}
