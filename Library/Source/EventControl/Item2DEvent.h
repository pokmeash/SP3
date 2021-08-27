#pragma once
#include "AbstractEvent.h"
#include "../App/Source/Scene2D/InventoryManager.h"

class Item2DEvent : public Event {
protected:
	CInventoryItem* item;
	const char* itemName;
public:
	Item2DEvent(const char* itemName, CInventoryItem* item) : itemName(itemName), item(item), Event(BASE_NAME()) {}
	CInventoryItem* getItem() {
		return item;
	}
	const char* getItemName() {
		return itemName;
	}
	static const std::string BASE_NAME() {
		return "Item2DEvent";
	}
};