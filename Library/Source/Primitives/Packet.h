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
	EntityPacket(long frame, CEntity2D* entity) 
		: entity(entity)
		, returnActiveState(false)
		, position(entity->vec2WSCoordinate)
		, velocity(entity->vec2Velocity)
		, counter(entity->counter) {
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
	glm::vec2 getVelocity() {
		return velocity;
	}
	bool isReturnActive() {
		return returnActiveState;
	}
	CEntity2D* getEntity() {
		return entity;
	}
	int getCounter() {
		return counter;
	}
protected:
	CEntity2D* entity;
	bool returnActiveState;
	glm::vec2 position;
	glm::vec2 velocity;
	int counter;
};

class BlockPacket : public Packet {
public:
	BlockPacket(long frame, glm::vec2 pos, int tile, CFloor2D* floor, int roomID) : position(pos), tile(tile), floor(floor), roomID(roomID) {
		this->frame = frame;
	}
	virtual ~BlockPacket() {}
	glm::vec2 getPosition() {
		return position;
	}
	int getTile() {
		return tile;
	}
	CFloor2D* getFloor() {
		return floor;
	}
	int getRoomID() {
		return roomID;
	}
protected:
	glm::vec2 position;
	int tile;
	CFloor2D* floor;
	int roomID;
};