#pragma once
#include "Player2DEvent.h"

class Player2DInteractEvent : public Player2DEvent, public Cancellable {
protected:
	int tileID;
public:
	Player2DInteractEvent(CPlayer2D* player, int tile) : Player2DEvent(player) {
		this->tileID = tile;
		Event::name = BASE_NAME();
	}

	int getTile() {
		return tileID;
	}

	static const std::string BASE_NAME() {
		return "Player2DInteractEvent";
	}
};