#pragma once
#include "AbstractEvent.h"
#include <includes/glm.hpp>

class Block2DChangeEvent : public Event, public Cancellable {
protected:
	int prev;
	int curr;
	glm::vec2 position;
public:
	Block2DChangeEvent(int prev, int next, glm::vec2 pos) : Event(BASE_NAME()), prev(prev), curr(next), position(pos) {
		name = BASE_NAME();
	}
	int getPreviousTile() {
		return prev;
	}
	int getNextTile() {
		return curr;
	}
	glm::vec2& getPosition() {
		return position;
	}
	static const std::string BASE_NAME() {
		return "Block2DChangeEvent";
	}
};
