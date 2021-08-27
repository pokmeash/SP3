#pragma once
#include "Player2DEvent.h"

class Player2DInventoryFullEvent : public Player2DEvent {
public:
	Player2DInventoryFullEvent(CPlayer2D* player) : Player2DEvent(player) {
		Event::name = BASE_NAME();
	};

	static const std::string BASE_NAME() {
		return "Player2DInventoryFullEvent";
	}
};