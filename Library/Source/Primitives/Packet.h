#pragma once
#include "Entity2D.h"

class Packet {
public:
	Packet() : frame(0) {}
	virtual ~Packet() {}
	long getFrame() {
		return frame;
	}
protected:
	long frame;
};

class EntityPacket : public Packet {
public:
	EntityPacket(long frame, CEntity2D* entity) : entity(entity), returnActiveState(false), position(entity->vec2WSCoordinate) {
		this->frame = frame;
	}
	virtual ~EntityPacket() {
		entity = NULL;
	}
	void setActiveState(bool bActive) {
		returnActiveState = bActive;
	}
	void setPosition(glm::vec2 pos) {
		position = pos;
	}
	glm::vec2 getPosition() {
		return position;
	}
	bool isReturnActive() {
		return returnActiveState;
	}
	CEntity2D* getEntity() {
		return entity;
	}
protected:
	CEntity2D* entity;
	bool returnActiveState;
	glm::vec2 position;
};

class BlockPacket : public Packet {
public:
	BlockPacket(long frame, glm::vec2 pos, int tile) : position(pos), tile(tile) {
		this->frame = frame;
	}
	virtual ~BlockPacket() {}
	glm::vec2 getPosition() {
		return position;
	}
	int getTile() {
		return tile;
	}
protected:
	glm::vec2 position;
	int tile;
};