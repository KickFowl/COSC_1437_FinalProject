#include "EventHandle.h"


void EventHandle::killSlot(eventPtr slot)
{
	delete slot;
	slot = nullptr;
}

eventPtr EventHandle::addEvent()
{
	//Looks for an empty slot
	auto it = std::find(eventStack.begin(), eventStack.end(), nullptr);

	//Allocate memory
	eventPtr newEvent = new Event();

	if (it != eventStack.end())
	{
		*it = newEvent; //insert into tombstone
	}
	else
	{
		eventStack.push_back(newEvent); //Append if no tombstone
	}

	return newEvent;
}

void EventHandle::ageEvents()
{
	for (eventPtr& slot : eventStack)
	{
		if (slot == nullptr || slot->immortal) continue;

		slot->currentLife++;
		if (slot->currentLife > slot->lifeSpan) killSlot(slot);
	}
}

bool EventHandle::killEvent(eventPtr ptr)
{
	auto it = std::find(eventStack.begin(), eventStack.end(), ptr);
	if (*it)
	{
		killSlot(*it);
		return true;
	}
	return false;
}

void EventHandle::clearEvents()
{
	for (eventPtr i : eventStack)
	{
		delete i;
	}
	eventStack.clear();
}

void EventHandle::listEvents()
{
	for (eventPtr i : eventStack)
	{
		if(i) i->print();
	}
}

void EventHandle::printEvent(eventPtr event)
{
	event->print();
}

std::vector<eventPtr> EventHandle::getAllEventofType(const Events Type) const
{
	std::vector<eventPtr> list;

	for (eventPtr i : eventStack)
	{
		if (i != nullptr && i->type == Type) list.push_back(i);
	}

	return list;
}

eventPtr EventHandle::getFirstofType(const Events Type) const
{
	for (eventPtr i : eventStack)
	{
		if (i && i->type == Type) return i;
	}
}

EventHandle::~EventHandle()
{
	clearEvents();
}
