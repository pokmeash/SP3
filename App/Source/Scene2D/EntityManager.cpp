#include "EntityManager.h"
#include "Scene2D.h"
#include "../Library/Source/System/MyMath.h"
#include "EventControl/EventHandler.h"

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
					break;
				case CEntity2D::E_BULLET:
				{
					for (unsigned j = 0; j < CScene2D::GetInstance()->enemyVector.size(); ++j)
					{
						CLivingEntity* enemy = (CLivingEntity*) CScene2D::GetInstance()->enemyVector[j];
						if (!enemy->bIsActive) continue;
						if (cPhysics.CalculateDistance(entity->vec2WSCoordinate, enemy->vec2WSCoordinate) <= 1)
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


