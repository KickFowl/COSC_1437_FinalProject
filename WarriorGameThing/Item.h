#pragma once

#include <string>

#include "defenitions.h"
#include "EventHandle.h"

class Item
{
	std::string name = "";

	Rarity rarity = Rarity::JUNK;

	int atk, def, dmg;

	unsigned int price;

	EventHandle& eventHandler;

protected:

	//setters
	void setATK(int attack) { atk = attack; }
	void setDEF(int defense) { def = defense; }
	void setDMG(int damage) { dmg = damage; }

	void setPrice(int newPrice) { price = newPrice; }
	void setRarity(Rarity newRarity) { rarity = newRarity; }

public:

	//TODO: In default constructor set all stats to NULL
	Item(EventHandle& eventHandler);

	//getters
	int getATK() const { return atk; }
	int getDEF() const { return def; }
	int getDMG() const { return dmg; }

	unsigned int getPrice() const { return price; }

	Rarity getRarity() const { return rarity; }

	virtual void sellItem() = 0;

	virtual ~Item() = default;
};

class Weapon : public Item
{
	//Generates random name, stats, and price
	void generateWeapon();

public:

	//TODO: Make default constructor generate random stats and name
	Weapon();
	Weapon(std::string name, int atk = 0, int dmg = 0, Rarity rarity = Rarity::UNIQUE);

	//sends attack event to event handler
	void attack();

	//sends sell event to event handler
	void sellItem();

	~Weapon() = default;
};

class Armor : public Item
{
	Slot slot;
	void generateArmor();
public:

	//need to make sure slot is assigned
	Armor();
	Armor(std::string name, int def = 0, Slot slot = Slot::CHEST, Rarity rarity = Rarity::UNIQUE);

	void useItem();
	void sellItem();

	//getter
	Slot getSlot() const { return slot; } //No need for a setter, this won't change

	//TODO: hehehehe, this could be fun. Maybe introduce a way to choose wich armor to ablate
	void ablateArmor();

	~Armor() = default;
};

//TODO: Impliment some kind of effect system for consumables
class Consumable : public Item
{

};
