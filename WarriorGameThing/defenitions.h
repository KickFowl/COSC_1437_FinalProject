#pragma once

#include <windows.h>
#include <string>
#include <cstdint>
#include <iostream>

using ENTITY_ID = std::uint8_t;

constexpr ENTITY_ID ENGINE = 0;

enum class Rarity {
	JUNK, COMMON, UNCOMMON, RARE, UNIQUE
};

enum class Slot {
	HEAD, CHEST, LEGS, ARM, BOOT
};

enum class Events {
	EMPTY, RESOURCE_CHANGE, STAT_CHANGE, ESCAPE,
	KEY_DOWN,
	MOUSE_MOVE, MOUSE_WHEEL,
	MOUSE_LEFT_DOWN, MOUSE_LEFT_UP, MOUSE_LEFT_TAP,
	MOUSE_RIGHT_DOWN, MOUSE_RIGHT_UP
};

inline std::wstring EventsToWstring(const Events& event)
{
	switch (event)
	{
	case Events::EMPTY: return L"EMPTY";
	case Events::RESOURCE_CHANGE: return L"RESOURCE_CHANGE";
	case Events::STAT_CHANGE: return L"STAT_CHANGE";
	default: return L"INAVLID_ENUM";
	}
}

inline std::string EventsToString(const Events& event)
{
	switch (event)
	{
	case Events::EMPTY: return "EMPTY";
	case Events::RESOURCE_CHANGE: return "RESOURCE_CHANGE";
	case Events::STAT_CHANGE: return "STAT_CHANGE";
	default: return "INAVLID_ENUM";
	}
}

inline std::wstring boolToWstring(bool item)
{
	return (item) ? L"True" : L"False";
}

struct Event
{
	Events type = Events::EMPTY;
	bool immortal = false;

	int lifeSpan = 1;
	int currentLife = 0;

	ENTITY_ID target = NULL;
	bool force = false;

	virtual ~Event() = default;

	virtual void print()
	{
		std::wstring eventInfo = L"EVENT:" + EventsToWstring(type) + L"\n";
		OutputDebugString(eventInfo.c_str());
		std::wstring eventInfo_1 = L"Immortal: " + boolToWstring(immortal) + L"\nForce: " + boolToWstring(force) + L"\n";
		OutputDebugString(eventInfo_1.c_str());
		std::wstring eventInfo_2 = L"Target: " + std::to_wstring(target) + L"\n";
		OutputDebugString(eventInfo_2.c_str());
		std::wstring eventInfo_3 = L"Lifespan: " + std::to_wstring(lifeSpan) + L" Current Life: " + std::to_wstring(currentLife) + L"\n";
		OutputDebugString(eventInfo_3.c_str());
	};
};

template <typename METADATA>
struct MetaEvent : public Event {
	METADATA metadata;

	MetaEvent(METADATA c) : metadata(c) {};

	void print() override
	{
		Event::print();

		OutputDebugString(L"\nHas Metadata\n\n");
	}
};

using eventPtr = Event*;