#pragma once
#include "Item2DEvent.h"
#include <includes/glm.hpp>

class Item2DPickUpEvent : public Item2DEvent, public Cancellable {
protected:
	glm::vec2 pos;
public:
	Item2DPickUpEvent(const char* itemName, CInventoryItem* item, glm::vec2 pos) : Item2DEvent(itemName, item), pos(pos) {
		name = BASE_NAME();
	}
	glm::vec2& getPosition() {
		return pos;
	}
	static const std::string BASE_NAME() {
		return "Item2DPickUpEvent";
	}
};