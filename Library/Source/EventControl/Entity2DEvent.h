#pragma once
#include "AbstractEvent.h"
#include "../Primitives/Entity2D.h"

class Entity2DEvent : public Event {
protected:
	CEntity2D* entity;
public:
	Entity2DEvent(CEntity2D* entity) : Event(BASE_NAME()) {
		this->entity = entity;
	}

	virtual CEntity2D* getEntity() {
		return entity;
	}

	static const std::string BASE_NAME() {
		return "Entity2DEvent";
	}
};