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
	void saveBlockPacket(int tile, glm::vec2 position, CFloor2D* floor) {
		BlockPacket* packet = new BlockPacket(currentFrame, position, tile, floor, floor->GetCurrentLevel());
		packets.push_back(packet);
	}

	void Init(void) {
		Reset();
		listening = false;
		EventHandler::GetInstance()->IgnoreCancelOn([&](Event* e) {
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
				std::cout << e->getName() << std::endl;
				saveBlockPacket(ev->getPreviousTile(), ev->getPosition(), CFloorManager::GetInstance()->GetCurrentFloor());
			}
		});
	}

	void Update() {
		currentFrame++;
	}

	bool UpdateReverse() {
		if (packets.size() == 0 || listening) {
			return false;
		}
		for (Packet* packet : packets) {
			if (packet->getFrame() == currentFrame) {
				if (dynamic_cast<EntityPacket*>(packet)) {
					EntityPacket* entityPacket = (EntityPacket*)packet;
					entityPacket->getEntity()->vec2WSCoordinate = entityPacket->getPosition();
					entityPacket->getEntity()->vec2Velocity = entityPacket->getVelocity();
					entityPacket->getEntity()->counter = entityPacket->getCounter();
					entityPacket->getEntity()->rotation = atan2f(entityPacket->getEntity()->vec2Velocity.y, entityPacket->getEntity()->vec2Velocity.x);
					CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->x, entityPacket->getEntity()->vec2WSCoordinate.x, &entityPacket->getEntity()->i32vec2Index.x, &entityPacket->getEntity()->i32vec2NumMicroSteps.x);
					CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->y, entityPacket->getEntity()->vec2WSCoordinate.y, &entityPacket->getEntity()->i32vec2Index.y, &entityPacket->getEntity()->i32vec2NumMicroSteps.y);
					if (entityPacket->isReturnActive() && !entityPacket->getEntity()->bIsActive) {
						entityPacket->getEntity()->bIsActive = true;
					} else if (entityPacket->isReturnActive() && entityPacket->getEntity()->bIsActive) {
						entityPacket->getEntity()->bIsActive = false;
					}
					continue;
				}
				if (dynamic_cast<BlockPacket*>(packet)) {
					BlockPacket* blockPacket = (BlockPacket*)packet;
					int temp = blockPacket->getFloor()->GetCurrentLevel();
					blockPacket->getFloor()->SetCurrentLevel(blockPacket->getRoomID());
					blockPacket->getFloor()->SetMapInfo(blockPacket->getPosition().y, blockPacket->getPosition().x, blockPacket->getTile());
					blockPacket->getFloor()->SetCurrentLevel(temp);
				}
			}
		}
		currentFrame--;
		return true;
	}
	
	void Reset(void) {
		for (Packet* packet : packets) {
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