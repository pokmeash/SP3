#pragma once
#include "DesignPatterns/SingletonTemplate.h"
#include "Primitives/Packet.h"
#include "EventControl/EventHandler.h"
#include <vector>

class CBossTimeControl : public CSingletonTemplate<CBossTimeControl> {
	friend class CSingletonTemplate<CBossTimeControl>;
public:
	void saveEntityPacket(CEntity2D* entity, bool respawn = false) {
		EntityPacket* packet = new EntityPacket(currentFrame, entity);
		packet->setPosition(glm::vec2(entity->vec2WSOldCoordinates));
		packet->setActiveState(respawn);
		packets.push_back(packet);
	}
	void saveBlockPacket(int tile, glm::vec2 position) {
		BlockPacket* packet = new BlockPacket(currentFrame, position, tile);
		packets.push_back(packet);
	}

	void Init(void) {
		Reset();
		listening = true;
		EventHandler::GetInstance()->On([&](Event* e) {
			std::cout << e->getName() << std::endl;
			if (!listening) return;
			if (e->getName() == Entity2DMoveEvent::BASE_NAME()) {
				saveEntityPacket(((Entity2DMoveEvent*)e)->getEntity());
			}
			if (e->getName() == Player2DMoveEvent::BASE_NAME()) {
				saveEntityPacket(((Player2DMoveEvent*)e)->getEntity());
			}
			if (e->getName() == Entity2DDespawnEvent::BASE_NAME() || e->getName() == Entity2DSpawnEvent::BASE_NAME()) {
				saveEntityPacket(((Entity2DDespawnEvent*)e)->getEntity(), true);
			}
			if (e->getName() == Block2DChangeEvent::BASE_NAME()) {
				Block2DChangeEvent* ev = (Block2DChangeEvent*)e;
				saveBlockPacket(ev->getNextTile(), ev->getPosition());
			}
		});
	}

	void Update() {
		currentFrame++;
	}

	void UpdateReverse() {
		currentFrame--;
	}
	
	void Reset(void) {
		for (auto packet : packets) {
			if (packet)
			delete packet;
		}
		packets.clear();
		currentFrame = 1;
	}

	std::vector<Packet*>& getPackets() {
		return packets;
	}

	bool isListening() {
		return listening;
	}

	void setListening(bool bListen) {
		listening = bListen;
	}

	long getCurrentFrame() {
		return currentFrame;
	}
protected:
	std::vector<Packet*> packets;
	CBossTimeControl() {
		listening = true;
		currentFrame = 1;
		packets.reserve(60);
	}
	virtual ~CBossTimeControl() {
		Reset();
	}
	bool listening;
	long currentFrame;
};