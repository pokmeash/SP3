#pragma once
#include "Entity2DEvent.h"
#include "../App/Source/Scene2D/Player2D.h"

class Player2DEvent : public Entity2DEvent {
protected:
	CPlayer2D* player;
public:
	Player2DEvent(CPlayer2D* player) : Entity2DEvent(player) {
		this->entity = player;
		this->player = player;
		Event::name = BASE_NAME();
	}

	CPlayer2D* getPlayer() {
		return player;
	}

	static const std::string BASE_NAME() {
		return "Player2DEvent";
	}
};