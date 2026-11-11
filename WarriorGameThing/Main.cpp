// WarriorGameThing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <iostream>

#include "InputManager.h"
#include "EventHandle.h"
#include "defenitions.h"
#include "DisplayEngine.h"
#include "Game.h"

//void beforeMakingGameClassToHandleAllLogic()
//{
//	Armor basicArmor("Basic Armor", 10);
//	Weapon basicWeapon("Basic Weapon", 20, 1);
//	
//	Character mainCharacter("Main Character", 100, 10, basicWeapon, basicArmor);
//	mainCharacter.printStats();
//	
//	Character enemy1("Enemy1", 50, 2, 12, 12);
//	enemy1.printStats();
//	
//	mainCharacter.attack(enemy1);
//	enemy1.attack(mainCharacter);
//	
//	Character clonedCharacter = mainCharacter.clone(mainCharacter);
//	mainCharacter.printStats();
//	
//	Enemy realEnemy("Real Enemy Class", 100, 20, 5, 5);
//	Enemy otherEnemy(enemy1); 
//	
//	realEnemy.printStats();
//	realEnemy.attack(mainCharacter);
//	
//	realEnemy.shapeShift("Cat");
//	realEnemy.attack(mainCharacter);
//	
//	realEnemy.castSpell("Fireball", mainCharacter);
//	
//	realEnemy.cakeDestruct();
//	
//}

EventHandle hndl;
DisplayEngine canvas;
InputManager input(hndl);
Game game;

struct payload
{
	int x = 5;
	int y = 10;

	std::string text = "Fuck You!!!";
};

int main()
{
	while (game.isRunning())
	{
		input.poll();
		game.pollEvents(hndl);

		hndl.ageEvents();
	}
}