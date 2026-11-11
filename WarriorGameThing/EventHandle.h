#pragma once
#include <vector>
#include <algorithm>

#include "defenitions.h"

class EventHandle
{
	std::vector<eventPtr> eventStack;

	void killSlot(eventPtr slot);
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

		if (it != eventStack.end()) *it = newEvent; //insert into tombstone
		else eventStack.push_back(newEvent); //Append if no tombstone

		return newEvent;
	}

	//goes through and adds 1 to the life of every event still in the stack.
	void ageEvents();

	//if an event reaches it's lifespan we kill it and remove it from the stack
	//Searches EventStack for event returning true after killing it
	bool killEvent(eventPtr ptr);
	//Kills every event
	void clearEvents();

	//lists all current events
	void listEvents();

	//Helper function for debugging
	void printEvent(eventPtr event);

	//getters

	//polling functions
	std::vector<eventPtr> getAllEventofType(const Events Type) const;
	eventPtr getFirstofType(const Events Type) const;

	~EventHandle();
};
