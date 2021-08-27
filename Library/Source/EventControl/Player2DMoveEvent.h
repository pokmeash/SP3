#pragma once
#include "Player2DEvent.h"

class Player2DMoveEvent : public Player2DEvent, public Cancellable {
protected:
	glm::vec2 toPos;
	glm::vec2 fromPos;
public:
	Player2DMoveEvent(CPlayer2D* player, glm::vec2 to, glm::vec2 from) : Player2DEvent(player) {
		toPos = to;
		fromPos = from;
		Event::name = BASE_NAME();
	}

	Player2DMoveEvent(CPlayer2D* player, glm::i32vec2 to, glm::i32vec2 from) : Player2DEvent(player) {
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
		return "Player2DMoveEvent";
	}
};
