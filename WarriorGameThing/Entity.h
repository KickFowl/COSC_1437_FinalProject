#pragma once

#include <string>

enum class resources {
	HP, MP, SP
};

class Entity
{
	std::string name = "";

	int maxHP, maxMP, maxSP, hp, mp, sp;

	bool alive = true;

public:

	Entity() = default;
	Entity(int hp, int mp, int sp) : maxHP(hp), maxMP(mp), maxSP(sp) { refillResources(); };
	
	//sets hp, mp, and sp to their max
	void refillResources();

	//should chack if alive and if hp is below zero
	bool isALive();

	//void because you can take infinite damage no check needed
	void takeDamage(int damage);

	//returns bool so in game logic can deny use of skill/spell
	bool spendMP(int cost);
	bool spendSP(int cost);

	//returns wether or not specified resource is overfull
	bool isOverfull(resources resource) const;

	//basic getters
	std::string getName() const { return name; }

	int getMaxHP() const { return maxHP; }
	int getMaxMP() const { return maxMP; }
	int getMaxSP() const { return maxSP; }
	int getHP() const { return hp; }
	int getMP() const { return mp; }
	int getSP() const { return sp; }

	//basic setters
	void setName(std::string newName) { name = newName; }

	~Entity() = default;
};

