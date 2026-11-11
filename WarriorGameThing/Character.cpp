//#include "Character.h"
//#include <iostream>
//#include <string>
//#include <map>
//#include <vector>
//
////////////////////////////////////////////////////////
//#pragma region Weapon
//Weapon::Weapon() = default;
//
//Weapon::Weapon(std::string name, unsigned int damage, unsigned int attackTurns)
//	:
//	name(name),damage(damage),attackTurns(attackTurns)
//{
//}
//
//int Weapon::getWeaponDamage() const
//{
//	return damage;
//}
//#pragma endregion
//
////////////////////////////////////////////////////////
//#pragma region Character
//Character::Character() = default;
//
//Character::Character(std::string name, int health, int gold, unsigned int defense, unsigned int damage)
//	:
//	name(name), health(health), gold(gold), defense(defense), damage(damage)
//{
//}
//
//Character::Character(std::string name, int health, int gold, const Weapon& equippedWeapon, const Armor& equippedArmor)
//	:
//	name(name), health(health), gold(gold), equippedWeapon(equippedWeapon),
//	equippedArmor(equippedArmor)
//{
//}
//
//void Character::setCharacterEquippedWeapon(const Weapon& weapon)
//{
//	equippedWeapon = weapon;
//	//damage = equippedWeapon.getWeaponDamage(); // double the damage stat :(
//}
//
//void Character::setCharacterEquippedArmor(const Armor& armor)
//{
//	equippedArmor = armor;
//	//defense = equippedArmor.getDefenseStat();
//	//doing this doulble the defense stat because the defense is already being set in the constructor and then again here. :(
//}
//
//void Character::attack(Character& enemy1)
//{
//	int actualDamage = damage + equippedWeapon.getWeaponDamage() - enemy1.defense;
//	if (actualDamage < 0) actualDamage = 0; // Prevent negative damage from healing!
//	enemy1.health -= actualDamage;
//
//	std::cout << "\n";
//	std::cout << name << " attacked: " << enemy1.name << " for: " << actualDamage << " damage\n";
//	std::cout << enemy1.name << " has: " << enemy1.health << " health left\n";
//
//}
//
//void Character::takeSpellDamage(int damageTaken)
//{
//	int actualDamage = damageTaken - defense;
//	if (actualDamage < 0) actualDamage = 0; // Prevent negative damage from healing!
//
//	health -= actualDamage;
//
//	std::cout << "\n" << name << " took " << actualDamage << " spell damage!\n";
//	std::cout << name << " has " << health << " health left.\n";
//}
//
//Character Character::clone(const Character& clonedPerson)	
//{
//	std::cout << "\n" << "Cloning " << clonedPerson.name << " into a new character!" << std::endl;
//
//	Character clonedCharacter;
//	clonedCharacter.name = clonedPerson.name + " (Clone)";
//	clonedCharacter.health = clonedPerson.health;
//	clonedCharacter.gold = clonedPerson.gold;
//	clonedCharacter.defense = clonedPerson.defense;
//	clonedCharacter.damage = clonedPerson.damage;
//	
//	std::cout << "New character name: " << clonedCharacter.name << std::endl;
//	clonedCharacter.printStats();
//	return clonedCharacter;
//}
//
//void Character::printStats()
//{
//	std::system("cls");
//	std::cout << "\n" << "------ " << name << " STATS ------" << "\n";
//	std::cout << "Name is: " << name << "\n";
//	std::cout << "Gold is: " << gold << "\n";
//	std::cout << "Health is: " << health << "\n";
//	std::cout << "Damage is: " << equippedWeapon.getWeaponDamage() + damage ;
//	std::cout << "\n";
//	std::cout << "Defense is: " << equippedArmor.getDefenseStat() + defense ;
//	std::cout << "\n";
//}
//
//#pragma endregion 
//
///////////////////////////////////////////////////////////////
//#pragma region Armor
//Armor::Armor() = default;
//
//Armor::Armor(std::string name, unsigned int defense)
//	:
//	name(name), 
//	defense(defense)
//{
//}
//
//int Armor::getDefenseStat() const
//{
//	return defense;
//}
//#pragma endregion
//
////////////////////////////////////////////////////////
//#pragma region Enemy
//Enemy::Enemy(Character character)
//{
//	//gold = character.gold; 
//	//character.gold = 123; 
//
//
//}
//
//Enemy::Enemy(std::string name, int health, int gold, unsigned int defense, unsigned int damage)
//	:
//	Character(name, health, gold, defense, damage)
//{
//}
//
////////////////////////////////////////////////////////////
//void Enemy::cakeDestruct()
//{
//	health = 0;
//	std::cout << "\n" << name << " ate the cake and died!!" << std::endl;
//}
//
//void Enemy::shapeShift(std::string animal)
//{
//
//	auto animalIterator = animalMap.find(animal); //finds the animal string in the map
//
//	if (animalIterator != animalMap.end()) //past the end of the map (doesnt exist)
//	{
//		int healthToSet = animalIterator->second.hp; //accessing the second value mapped {HP,Attack} // gets the pair value of the animal found in the map
//		int damageToSet = animalIterator->second.attack;
//
//		health = healthToSet;
//		damage = damageToSet;
//
//		std::cout << "\n" << name << " turned into a: " << animal << "!!\n";
//		std::cout << "Health:" << health << "\t" << "Damage:" << damage << std::endl;
//	}
//	else
//	{
//		std::cout << "You summoned the wrong animal! CAST FAILED" << std::endl;
//	}
//}
//
//void Enemy::castSpell(std::string spellName, Character& target)
//{
//	auto spellIterator = spellMap.find(spellName);
//
//	if (spellIterator != spellMap.end())
//	{
//		unsigned int spellDamage = spellIterator->second; //accessing the map pair then getting the second value (damage) of the pair
//		target.takeSpellDamage(spellDamage); //calls the takeSpellDamage from the Character class and applies the damage to the target character
//	}
//	else
//	{
//		std::cout << "You cast the wrong spell! CAST FAILED" << std::endl;
//	}
//}
//#pragma endregion