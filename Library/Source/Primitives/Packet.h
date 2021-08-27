#pragma once
#include "Entity2D.h"
#include "SpriteAnimation.h"

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
		, counter(entity->counter)
		, timer(entity->timer) 
		, health(0) 
		, damage(0) 
		, maxHealth(0) {
		this->frame = frame;
		if (dynamic_cast<CLivingEntity*>(entity)) {
			CLivingEntity* living = (CLivingEntity*)entity;
			damage = living->getDmg();
			health = living->getMaxHP();
			maxHealth = living->getMaxHP();
		}
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
	float getTimer() {
		return timer;
	}
	float getHealth() {
		return health;
	}
	float getDamage() {
		return damage;
	}
	float getMaxHealth() {
		return maxHealth;
	}
protected:
	CEntity2D* entity;
	bool returnActiveState;
	glm::vec2 position;
	glm::vec2 velocity;
	int counter;
	float timer;
	float health;
	float maxHealth;
	float damage;
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

class AnimationPacket : public Packet {
public:
	AnimationPacket(long frame, CSpriteAnimation* sprite, CAnimation* animation, int animFrame, bool returnActive) : sprite(sprite), animation(animation), animFrame(animFrame), returnActiveState(returnActive) {
		this->frame = frame;
		playCount = sprite->playCount;
		currentTime = sprite->currentTime;
	}
	CAnimation* getAnimation() {
		return animation;
	}
	CSpriteAnimation* getSprite() {
		return sprite;
	}
	int getAnimFrame() {
		return animFrame;
	}
	bool isReturnActive() {
		return returnActiveState;
	}
	int getPlayCount() {
		return playCount;
	}
	float getTime() {
		return currentTime;
	}
protected:
	CSpriteAnimation* sprite;
	CAnimation* animation;
	int animFrame;
	bool returnActiveState;
	int playCount;
	float currentTime;
};