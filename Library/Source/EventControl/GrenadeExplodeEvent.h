#pragma once
#include "Entity2DEvent.h"

class GrenadeExplodeEvent : public Entity2DEvent {
public:
	GrenadeExplodeEvent(CEntity2D* grenade) : Entity2DEvent(grenade) {
		name = BASE_NAME();
	}
	static const std::string BASE_NAME() {
		return "GrenadeExplodeEvent";
	}
};
