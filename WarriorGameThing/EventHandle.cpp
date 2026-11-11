#include "EventHandle.h"


std::uint8_t EventHandle::assignEventID()
{
	if (!freeID.empty())
	{
		uint8_t id = freeID.back();
		freeID.pop_back();
		return id;
	}

	return ++maxID;
}

void EventHandle::killSlot(eventPtr& slot)
{
	freeID.push_back(slot->id);
	delete slot;
	slot = nullptr;
}

eventPtr EventHandle::addEvent()
{
	//Looks for an empty slot
	auto it = std::find(eventStack.begin(), eventStack.end(), nullptr);

	//Allocate memory
	eventPtr newEvent = new Event();
	newEvent->id = assignEventID();

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

bool EventHandle::killEvent(int id)
{
	for (auto it = eventStack.begin(); it != eventStack.end(); ++it)
	{
		if (*it != nullptr && (*it)->id == id)
		{
			killSlot(*it);
			return true;
		}
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
	freeID.clear();
	maxID = 0;
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

std::uint8_t EventHandle::getEventID(eventPtr event)
{
	auto it = std::find(eventStack.begin(), eventStack.end(), event);
	if (it != eventStack.end()) {
		return (*it)->id;
	}

	return 255;
}

const Events EventHandle::getType(eventPtr event)
{
	auto it = std::find(eventStack.begin(), eventStack.end(), event);
	if (it != eventStack.end()) {
		return (*it)->type;
	}

	return Events::EMPTY;
}

std::vector<std::uint8_t> EventHandle::getAllEventofType(const Events Type) const
{
	std::vector<std::uint8_t> list;

	for (eventPtr i : eventStack)
	{
		if (i->type == Type) list.push_back(i->id);
	}

	return list;
}

EventHandle::~EventHandle()
{
	clearEvents();
}
