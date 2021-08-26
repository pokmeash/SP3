#pragma once
#include "Player2DEvent.h"

class Player2DDeathEvent : public Player2DEvent, public Cancellable {
protected:
	glm::vec2 death;
	glm::vec2 respawn;
public:
	Player2DDeathEvent(CPlayer2D* player, glm::vec2 deathLocation, glm::vec2 respawnLocation) : Player2DEvent(player) {
		this->death = deathLocation;
		this->respawn = respawnLocation;
		Event::name = BASE_NAME();
	}

	Player2DDeathEvent(CPlayer2D* player, glm::i32vec2 deathLocation, glm::i32vec2 respawnLocation) : Player2DEvent(player) {
		this->death = deathLocation;
		this->respawn = respawnLocation;
		Event::name = BASE_NAME();
	}

	glm::vec2& getDeathLocation() {
		return death;
	}

	glm::vec2& getRespawnLocation() {
		return respawn;
	}

	static const std::string BASE_NAME() {
		return "Player2DDeathEvent";
	}
};
