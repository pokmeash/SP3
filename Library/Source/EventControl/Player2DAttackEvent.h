#pragma once
#include "Player2DEvent.h"

class Player2DAttackEvent : public Player2DEvent, public Cancellable {
protected:
	float damage;
public:
	Player2DAttackEvent(CPlayer2D* entity, float damage) : Player2DEvent(entity) {
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
		return "Player2DAttackEvent";
	}
};