#pragma once
#include <vector>
#include <algorithm>

#include "defenitions.h"

class EventHandle
{
	std::uint8_t maxID = 0;
	std::vector<std::uint8_t> freeID;

	std::vector<eventPtr> eventStack;

	std::uint8_t assignEventID();
	void killSlot(eventPtr& slot);
public:
	//used to add events across the game
	eventPtr addEvent();

	template <typename T>
	eventPtr addEvent(T metadata)
	{
		//Looks for an empty slot
		auto it = std::find(eventStack.begin(), eventStack.end(), nullptr);

		//Allocate memory
		eventPtr newEvent = new MetaEvent<T>(metadata);
		newEvent->id = assignEventID();

		if (it != eventStack.end()) *it = newEvent; //insert into tombstone
		else eventStack.push_back(newEvent); //Append if no tombstone

		return newEvent;
	}

	//goes through and adds 1 to the life of every event still in the stack.
	void ageEvents();

	//if an event reaches it's lifespan we kill it and remove it from the stack
	//Searches EventStack for event with matching id and then deletes and returns true
	bool killEvent(int id);
	//Kills every event
	void clearEvents();

	//lists all current events
	void listEvents();

	//Helper function for debugging
	void printEvent(eventPtr event);

	//getters
	std::uint8_t getEventID(eventPtr event);
	const Events getType(eventPtr event);

	//polling functions
	std::vector<std::uint8_t> getAllEventofType(const Events Type) const;

	~EventHandle();
};
