#pragma once
#include "Entity2DEvent.h"

class Entity2DMoveEvent : public Entity2DEvent, public Cancellable {
protected:
	glm::vec2 toPos;
	glm::vec2 fromPos;
public:
	Entity2DMoveEvent(CEntity2D* entity, glm::vec2 to, glm::vec2 from) : Entity2DEvent(entity) {
		toPos = to;
		fromPos = from;
		Event::name = BASE_NAME();
	}

	Entity2DMoveEvent(CEntity2D* entity, glm::i32vec2 to, glm::i32vec2 from) : Entity2DEvent(entity) {
		toPos = to;
		fromPos = from;
		Event::name = BASE_NAME();
	}

	glm::vec2& getTo() {
		return toPos;
	}

	glm::vec2& getFrom() {
		return fromPos;
	}

	static const std::string BASE_NAME() {
		return "Entity2DMoveEvent";
	}
};
