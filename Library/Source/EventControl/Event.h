#ifndef EVENT_H
#define EVENT_H

#include <string>
#include "../Primitives/Entity2D.h"
#include "../Primitives/Entity3D.h"
#include "../App/Source/Scene2D/Player2D.h"
#include "../App/Source/Scene2D/InventoryItem.h"

class Event {
protected:
	std::string name;
public:
	Event(const std::string& name): name(name) {}

	virtual ~Event() {}

	std::string getName() {
		return name;
	}
};

class Cancellable {
protected:
	bool cancelled;
public:
	Cancellable(): cancelled(false) {}
	bool isCancelled() {
		return cancelled;
	}

	void setCancelled(bool cancel) {
		cancelled = cancel;
	}
};

class Entity2DEvent : public Event {
protected:
	CEntity2D* entity;
public:
	Entity2DEvent(CEntity2D* entity) : Event(BASE_NAME()) {
		this->entity = entity;
	}

	virtual CEntity2D* getEntity() {
		return entity;
	}

	static const std::string BASE_NAME() {
		return "Entity2DEvent";
	}
};

class Projectile2DLaunchEvent : public Entity2DEvent, public Cancellable {
protected:
	CEntity2D* projectile;
	glm::vec2 direction;
public:
	Projectile2DLaunchEvent(CEntity2D* projectile, glm::vec2& direction) : Entity2DEvent(projectile) {
		Event::name = BASE_NAME();
		this->direction = direction;
	}

	Projectile2DLaunchEvent(CEntity2D* projectile, glm::i32vec2& direction) : Entity2DEvent(projectile) {
		Event::name = BASE_NAME();
		this->direction = direction;
	}

	CEntity2D* getProjectile() {
		return projectile;
	}

	glm::vec2& getDirection() {
		return direction;
	}

	static const std::string BASE_NAME() {
		return "Projectile2DLaunchEvent";
	}
};

class Entity2DMoveEvent : public Entity2DEvent, public Cancellable {
protected:
	glm::vec2 toPos;
	glm::vec2 fromPos;
public:
	Entity2DMoveEvent(CEntity2D* entity, glm::vec2& to, glm::vec2& from) : Entity2DEvent(entity) {
		toPos = to;
		fromPos = from;
		Event::name = BASE_NAME();
	}

	Entity2DMoveEvent(CEntity2D* entity, glm::i32vec2& to, glm::i32vec2& from) : Entity2DEvent(entity) {
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
		return "Entity2DMoveEvent";
	}
};

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

class Entity3DEvent : public Event {
protected:
	CEntity3D* entity;
public:
	Entity3DEvent(CEntity3D* entity) : Event(BASE_NAME()) {
		this->entity = entity;
	}

	CEntity3D* getEntity() {
		return entity;
	}

	static const std::string BASE_NAME() {
		return "Entity3DEvent";
	}
};

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

class Player2DPickUpEvent : public Player2DEvent, public Cancellable {
protected:
	CInventoryItem* item;
public:
	Player2DPickUpEvent(CPlayer2D* player, CInventoryItem* item) : Player2DEvent(player) {
		this->item = item;
		Event::name = BASE_NAME();
	}

	CInventoryItem* getItem() {
		return item;
	}

	static const std::string BASE_NAME() {
		return "Player2DPickUpEvent";
	}
};

class Player2DMoveEvent : public Player2DEvent, public Cancellable {
protected:
	glm::vec2 toPos;
	glm::vec2 fromPos;
public:
	Player2DMoveEvent(CPlayer2D* player, glm::vec2& to, glm::vec2& from) : Player2DEvent(player) {
		toPos = to;
		fromPos = from;
		Event::name = BASE_NAME();
	}

	Player2DMoveEvent(CPlayer2D* player, glm::i32vec2& to, glm::i32vec2& from) : Player2DEvent(player) {
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

class Player2DInventoryFullEvent : public Player2DEvent {
public:
	Player2DInventoryFullEvent(CPlayer2D* player) : Player2DEvent(player) {
		Event::name = BASE_NAME();
	};

	static const std::string BASE_NAME() {
		return "Player2DInventoryFullEvent";
	}
};

class Player2DDeathEvent : public Player2DEvent, public Cancellable {
protected:
	glm::vec2 death;
	glm::vec2 respawn;
public:
	Player2DDeathEvent(CPlayer2D* player, glm::vec2& deathLocation, glm::vec2& respawnLocation) : Player2DEvent(player) {
		this->death = deathLocation;
		this->respawn = respawnLocation;
		Event::name = BASE_NAME();
	}

	Player2DDeathEvent(CPlayer2D* player, glm::i32vec2& deathLocation, glm::i32vec2& respawnLocation) : Player2DEvent(player) {
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

#endif