#pragma once
#include "DesignPatterns/SingletonTemplate.h"
#include "Primitives/Packet.h"
#include "EventControl/EventHandler.h"
#include "EventControl/Entity2DMoveEvent.h"
#include "EventControl/Player2DMoveEvent.h"
#include "EventControl/Block2DChangeEvent.h"
#include "EventControl/Entity2DDespawnEvent.h"
#include "EventControl/Entity2DSpawnEvent.h"
#include "EventControl/AnimationFrameChangeEvent.h"
#include "EventControl/AnimationStopEvent.h"
#include "EventControl/AnimationStartEvent.h"
#include "EventControl/NextRoomEvent.h"
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

	void saveAnimationPacket(AnimationEvent* e, bool respawn = false) {
		AnimationPacket* packet = nullptr;
		if (dynamic_cast<AnimationFrameChangeEvent*>(e)) {
			AnimationFrameChangeEvent* ev = (AnimationFrameChangeEvent*)e;
			packet = new AnimationPacket(currentFrame, e->getSprite(), e->getAnimation(), ev->getPreviousFrame(), respawn);
		} else if (dynamic_cast<AnimationStopEvent*>(e)) {
			packet = new AnimationPacket(currentFrame, e->getSprite(), e->getAnimation(), -1, respawn);
		} else if (dynamic_cast<AnimationStartEvent*>(e)) {
			packet = new AnimationPacket(currentFrame, e->getSprite(), e->getAnimation(), -2, respawn);
		}
		packets.push_back(packet);
	}

	void Init(void) {
		Reset();
		if (listener) return;
		listener = EventHandler::GetInstance()->IgnoreCancelOn([&](Event* e) {
			if (!listening) return;
			if (e->getName() == NextRoomEvent::BASE_NAME()) {
				listening = false;
				Reset();
				return;
			}
			if (e->getName() == Entity2DMoveEvent::BASE_NAME()) {
				saveEntityPacket(((Entity2DMoveEvent*)e)->getEntity());
				return;
			}
			if (e->getName() == Player2DMoveEvent::BASE_NAME()) {
				saveEntityPacket(((Player2DMoveEvent*)e)->getEntity());
				return;
			}
			if (e->getName() == Entity2DDespawnEvent::BASE_NAME() || e->getName() == Entity2DSpawnEvent::BASE_NAME()) {
				saveEntityPacket(((Entity2DDespawnEvent*)e)->getEntity(), true);
				return;
			}
			if (e->getName() == Block2DChangeEvent::BASE_NAME()) {
				Block2DChangeEvent* ev = (Block2DChangeEvent*)e;
				saveBlockPacket(ev->getPreviousTile(), ev->getPosition(), CFloorManager::GetInstance()->GetCurrentFloor());
				return;
			}
			if (e->getName() == AnimationFrameChangeEvent::BASE_NAME()) {
				AnimationFrameChangeEvent* ev = (AnimationFrameChangeEvent*)e;
				saveAnimationPacket(ev);
				return;
			}
			if (e->getName() == AnimationStopEvent::BASE_NAME()) {
				AnimationStopEvent* ev = (AnimationStopEvent*)e;
				saveAnimationPacket(ev, true);
				return;
			}
			if (e->getName() == AnimationStartEvent::BASE_NAME()) {
				AnimationStartEvent* ev = (AnimationStartEvent*)e;
				saveAnimationPacket(ev, true);
				return;
			}
		});
	}

	void Update() {
		currentFrame++;
		if (currentFrame >= 180) listening = false;
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
					entityPacket->getEntity()->timer = entityPacket->getTimer();
					if (!dynamic_cast<CLivingEntity*>(entityPacket->getEntity())) {
						entityPacket->getEntity()->rotation = atan2f(entityPacket->getEntity()->vec2Velocity.y, entityPacket->getEntity()->vec2Velocity.x);
					} else {
						CLivingEntity* living = (CLivingEntity*)entityPacket->getEntity();
						living->setHP(entityPacket->getHealth());
						living->setMaxHP(entityPacket->getMaxHealth());
						living->setDmg(entityPacket->getDamage());
					}
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
					continue;
				}
				if (dynamic_cast<AnimationPacket*>(packet)) {
					AnimationPacket* animPacket = (AnimationPacket*)packet;
					if (animPacket->isReturnActive() && animPacket->getAnimFrame() == -1) {
						animPacket->getAnimation()->ended = false;
						animPacket->getAnimation()->animActive = true;
						std::cout << "stop anim\n";
					} else if (animPacket->isReturnActive() && animPacket->getAnimFrame() == -2) {
						animPacket->getAnimation()->ended = true;
						animPacket->getAnimation()->animActive = false;
						std::cout << "start anim\n";
					} else if (!animPacket->isReturnActive()) {
						std::cout << "currentframe: " << animPacket->getSprite()->currentFrame << std::endl;
						animPacket->getSprite()->currentFrame = animPacket->getAnimFrame();
						animPacket->getSprite()->playCount = animPacket->getPlayCount();
						animPacket->getSprite()->currentTime = animPacket->getTime();
					}
					continue;
				}
			}
		}
		currentFrame--;
		reversing = true;
		if (currentFrame <= 0) {
			Reset();
		}
		return true;
	}
	
	void Reset(void) {
		for (Packet* packet : packets) {
			if (packet)
			delete packet;
		}
		packets.clear();
		reversing = false;
		listening = false;
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

	bool isReversing() {
		return reversing;
	}
protected:
	std::vector<Packet*> packets;
	CBossTimeControl() {
		listener = NULL;
		listening = true;
		reversing = false;
		currentFrame = 1;
		packets.reserve(60);
	}
	virtual ~CBossTimeControl() {
		Reset();
		EventHandler::GetInstance()->Remove(listener);
	}
	bool listening;
	long currentFrame;
	bool reversing;
	Listener* listener;
};