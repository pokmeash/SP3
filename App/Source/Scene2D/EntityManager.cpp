#include "EntityManager.h"
#include "Scene2D.h"
#include "../Library/Source/System/MyMath.h"
#include "EventControl/EventHandler.h"
#include "EventControl/NextRoomEvent.h"
#include "EventControl/Entity2DDespawnEvent.h"
#include "EventControl/GrenadeExplodeEvent.h"

EntityManager::EntityManager(void)
{
}

EntityManager::~EntityManager(void)
{
}

bool EntityManager::Init(void)
{
	cMap2D = CFloorManager::GetInstance();
	EventHandler::GetInstance()->On([&](Event* e) {
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
		if (e->getName() == GrenadeExplodeEvent::BASE_NAME()) {
			Grenade* grenade = (Grenade*)((GrenadeExplodeEvent*)e)->getEntity();
			for (unsigned j = 0; j < CScene2D::GetInstance()->enemyVector.size(); ++j) {
				CLivingEntity* enemy = (CLivingEntity*)CScene2D::GetInstance()->enemyVector[j];
				if (!enemy->bIsActive) continue;
				if (cPhysics.CalculateDistance(grenade->vec2WSCoordinate, enemy->vec2WSCoordinate) <= 2.f) {
					enemy->addHP(-1);
					if (enemy->getHP() <= 0) {
						enemy->bIsActive = false;
						EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(enemy));
					}
				}
			}
			return;
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
					break;
				case CEntity2D::E_BULLET:
				{
					for (unsigned j = 0; j < CScene2D::GetInstance()->enemyVector.size(); ++j)
					{
						CLivingEntity* enemy = (CLivingEntity*) CScene2D::GetInstance()->enemyVector[j];
						if (!enemy->bIsActive) continue;
						if (cPhysics.CalculateDistance(entity->vec2WSCoordinate, enemy->vec2WSCoordinate) <= enemy->scale.x)
						{
							enemy->setHP(enemy->getHP() - CPlayer2D::GetInstance()->getDmg());
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
							enemy->setHP(enemy->getHP() - 1);
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

bool EntityManager::getExplode()
{
	return explode;
}

void EntityManager::setExplode()
{
	explode = !explode;
}

glm::vec2 EntityManager::getPos()
{
	return tempPos;
}


