#pragma once
#include "Entity2DEvent.h"

class Entity2DAttackEvent : public Entity2DEvent, public Cancellable {
protected:
	float damage;
public:
	Entity2DAttackEvent(CEntity2D* entity, float damage) : Entity2DEvent(entity) {
		this->damage = damage;
		Event::name = BASE_NAME();
	}

	float getDamage() {
		return damage;
	}

	void setDamage(float damage) {
		this->damage = damage;
	}

	static const std::string BASE_NAME() {
		return "Entity2DAttackEvent";
	}
};
