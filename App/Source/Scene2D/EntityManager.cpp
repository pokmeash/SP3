#include "EntityManager.h"
#include "Scene2D.h"
#include "System/MyMath.h"
#include "EventControl/EventHandler.h"
#include "EventControl/NextRoomEvent.h"
#include "EventControl/Entity2DDespawnEvent.h"
#include "EventControl/Entity2DMoveEvent.h"
#include "EventControl/Entity2DSpawnEvent.h"
#include "EventControl/GrenadeExplodeEvent.h"
#include "../SoundController/SoundController.h"

EntityManager::EntityManager(void) : listener(NULL), cMap2D(NULL)
{
}

EntityManager::~EntityManager(void)
{
	EventHandler::GetInstance()->Remove(listener);
}

bool EntityManager::Init(void)
{
	cMap2D = CFloorManager::GetInstance();
	listener = EventHandler::GetInstance()->On([&](Event* e) {
		if (e->getName() == NextRoomEvent::BASE_NAME()) {
			for (unsigned i = 0; i < entitylist.size(); ++i) {
				CEntity2D* entity = entitylist[i];
				if (entity) {
					delete entity;
				}
			}
			entitylist.clear();
			return;
		}
		if (e->getName() == Entity2DSpawnEvent::BASE_NAME()) {
			Entity2DSpawnEvent* ev = (Entity2DSpawnEvent*)e;
			if (ev->getEntity()->type == CEntity2D::E_GRENADE) {
				CSoundController::GetInstance()->Replay(CSoundController::SOUNDS::THROW);
			}
			return;
		}
		if (e->getName() == GrenadeExplodeEvent::BASE_NAME()) {
			Grenade* grenade = (Grenade*)((GrenadeExplodeEvent*)e)->getEntity();
			CSoundController::GetInstance()->Replay(CSoundController::SOUNDS::GRENADE);
			for (unsigned j = 0; j < CScene2D::GetInstance()->enemyVector.size(); ++j) {
				CLivingEntity* enemy = (CLivingEntity*)CScene2D::GetInstance()->enemyVector[j];
				if (!enemy->bIsActive) continue;
				float dist = glm::length(grenade->vec2WSCoordinate - enemy->vec2WSCoordinate);
				if (dist <= enemy->scale.x * 0.5f + 1.f) {
					enemy->addHP(-1 / dist);
					if (enemy->getHP() <= 0) {
						enemy->bIsActive = false;
						EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(enemy));
					}
				}
			}
			return;
		}
		if (e->getName() == Entity2DMoveEvent::BASE_NAME()) {
			Entity2DMoveEvent* ev = (Entity2DMoveEvent*)e;
			if (dynamic_cast<CEnemy2D*>(ev->getEntity())) {
				CEnemy2D* enemy = (CEnemy2D*)ev->getEntity();
				for (unsigned i = 0; i < CScene2D::GetInstance()->enemyVector.size(); ++i)
				{
					CLivingEntity* enemy2 = (CLivingEntity*)CScene2D::GetInstance()->enemyVector[i];
					if (!enemy2->bIsActive || enemy == enemy2) continue;
					if (glm::length(ev->getTo() - enemy2->vec2WSCoordinate) < 1.f)
					{
						ev->setCancelled(true);
						if (enemy->sCurrentFSM = CLivingEntity::FSM::MOVERIGHT)
						{
							enemy2->sCurrentFSM = CLivingEntity::FSM::MOVELEFT;
						}

						else if (enemy->sCurrentFSM = CLivingEntity::FSM::MOVELEFT)
						{
							enemy2->sCurrentFSM = CLivingEntity::FSM::MOVERIGHT;
						}
					}
				}
				return;
			}
		}
	});
	return false;
}

void EntityManager::Update(const double dElapsedTime)
{
	//iterator
	if (entitylist.size() > 0)
	{
		for (unsigned i = 0; i < entitylist.size(); ++i)
		{
			CEntity2D* entity = entitylist[i];
			if (entity->bIsActive)
			{
				entity->Update(dElapsedTime);
				if (entity->vec2WSCoordinate.x >= CSettings::GetInstance()->NUM_TILES_XAXIS || entity->vec2WSCoordinate.x < 0) entity->bIsActive = false;
				if (entity->vec2WSCoordinate.y >= CSettings::GetInstance()->NUM_TILES_YAXIS || entity->vec2WSCoordinate.y < 0) entity->bIsActive = false;
				if (!entity->bIsActive) continue;
				switch (entity->type)
				{
				case CEntity2D::E_EBULLET:
				{
					if (cPhysics.CalculateDistance(entity->vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) <= 1)
					{
						for (std::vector<CEntity2D*>::iterator it2 = CScene2D::GetInstance()->enemyVector.begin(); it2 != CScene2D::GetInstance()->enemyVector.end(); ++it2)
						{
							CLivingEntity* enemy = (CLivingEntity*)*it2;
							if (!enemy->bIsActive) continue;
							CPlayer2D::GetInstance()->PlayerDamaged(enemy->getDmg());
						}
						
						entity->bIsActive = false;
					}
					break;
				case CEntity2D::E_BULLET:
				{
					
					for (unsigned j = 0; j < CScene2D::GetInstance()->enemyVector.size(); ++j)
					{
						CLivingEntity* enemy = (CLivingEntity*)CScene2D::GetInstance()->enemyVector[j];
						if (!enemy->bIsActive) continue;
						if (cPhysics.CalculateDistance(entity->vec2WSCoordinate, enemy->vec2WSCoordinate) <= enemy->scale.x)
						{
							enemy->minusHP(CPlayer2D::GetInstance()->getDmg());
							CGameManager::GetInstance()->addFinalDmg(CPlayer2D::GetInstance()->getDmg());
							if (enemy->getHP() <= 0)
							{
								enemy->bIsActive = false;
								EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(enemy));
							}
							entity->bIsActive = false;
							EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(entity));
						}
					}
					break;
				}
				case CEntity2D::E_FRAGMENT:
					for (unsigned j = 0; j < CScene2D::GetInstance()->enemyVector.size(); ++j)
					{
						CLivingEntity* enemy = (CLivingEntity*)CScene2D::GetInstance()->enemyVector[j];
						if (!enemy->bIsActive) continue;
						if (cPhysics.CalculateDistance(entity->vec2WSCoordinate, enemy->vec2WSCoordinate) <= 1)
						{
							enemy->minusHP(CPlayer2D::GetInstance()->getDmg());
							CGameManager::GetInstance()->addFinalDmg(CPlayer2D::GetInstance()->getDmg());
							if (enemy->getHP() <= 0)
							{
								enemy->bIsActive = false;
								EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(enemy));
							}
							entity->bIsActive = false;
							EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(entity));
						}
					}
					break;
				case CEntity2D::E_GRENADE:
					break;
				case CEntity2D::E_BEAM:
					for (unsigned j = 0; j < entitylist.size(); ++j) {
						CEntity2D* entity2 = entitylist[j];
						if (!entity2->bIsActive || entity2 == entity) continue;
						if (entity2->type != CEntity2D::E_GRENADE) continue;
						Grenade* grenade = (Grenade*)entity2;
						if (glm::length(entity->vec2WSCoordinate - grenade->vec2WSCoordinate) <= .5f) {
							grenade->setExplode(true);
						}
					}
					break;
				default:
					break;
				}
				}
			}
		}
	}
}

void EntityManager::Render(void)
{
	if (entitylist.size() > 0)
	{
		for (std::vector<CEntity2D*>::iterator it = entitylist.begin(); it != entitylist.end(); ++it)
		{
			CEntity2D* entity = (CEntity2D*)*it;

			if (entity->bIsActive)
			{
				entity->PreRender();
				entity->Render();
				entity->PostRender();
			}
		}
	}
}
