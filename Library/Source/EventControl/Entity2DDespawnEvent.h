#pragma once
#include "Entity2DEvent.h"

class Entity2DDespawnEvent : public Entity2DEvent {
public:
	Entity2DDespawnEvent(CEntity2D* entity) : Entity2DEvent(entity) {
		name = BASE_NAME();
	}
	static std::string BASE_NAME() {
		return "Entity2DDespawnEvent";
	}
};