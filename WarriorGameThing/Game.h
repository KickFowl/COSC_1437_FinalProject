#pragma once

#include <iostream>
#include <fstream>
#include <windows.h>

#include "defenitions.h"
#include "EventHandle.h"
#include "Entity.h"
#include "RNG.h"


//TODO: Turn this into a full on state mechine
//Connect it to the events system
//determain states based on events
class Game
{

	bool running = true;
public:
	Game() = default; // default constructor

	inline bool isRunning() const { return running; }

	void pollEvents(EventHandle& hndl);
	
};