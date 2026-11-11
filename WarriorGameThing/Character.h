#pragma once

#include <string>
#include <map>
#include <vector>

#include "Entity.h"
#include "EventHandle.h"
#include "RNG.h"
#include "Item.h"


class Character : public Entity
{

protected:
	//unsigned int gold = 0;

	//re-wrote the classes and made these a refferance instead of an entire object
	//Weapon& equippedWeapon;
	//Armor& equippedArmor;

	/*
	*Unneeded as now damage and defense is handled by items
	*	unsigned int damage = equippedWeapon.getWeaponDamage() || 0;
	*	unsigned int defense = equippedArmor.getDefenseStat() || 0;
	*/

public:
	//TODO: Changed the constructors to match the new class structure
	//Character();
	//Character(std::string name, int health, int gold, unsigned int defense, unsigned int damage);
	//Character(std::string name, int health, int gold, const Weapon& equippedWeapon, const Armor& equippedArmor );

	//Changed setters to mach new structure
	//void setCharacterEquippedWeapon(const Weapon& weapon);
	//void setCharacterEquippedArmor(const Armor& armor);

	//functions
	//void attack(Character& enemy1);
	//void takeSpellDamage(int damageTaken);
	//Character clone(const Character& clonedPerson); //will return a new character so use the = operator to assign it to a new character object.
	//void printStats();
};

//TODO: Moved Enemy class to seperate file

//class Enemy : public Entity
//{
//	//using Character::Character; inherit all constructors from the base class Character.
//private:
//	// Since animalMap is now a member variable inside the Enemy class, 
//	// shapeShift already has direct access to it.
//	std::map<std::string, HPAttack> animalMap =
//	{
//		{"Cat", {120, 15}},
//		{"Dog", {100, 10}},
//		{"Dragon", {200, 25}},
//		{"Unicorn", {180, 20}},
//		{"Bear", {250, 18}},
//	};
//
//	std::map<std::string, unsigned int> spellMap =
//	{
//		{"Fireball", 30},
//		{"Ice Spike", 20},
//		{"Lightning Bolt", 25},
//		{"Stinky Cloud", 50},
//	};
//
//public:
//	
//	Enemy(Character character);
//	Enemy(std::string name, int health, int gold, unsigned int defense, unsigned int damage);
//
//	void cakeDestruct();
//	void shapeShift(std::string animal);
//	void castSpell(std::string spellName, Character& target);
//};