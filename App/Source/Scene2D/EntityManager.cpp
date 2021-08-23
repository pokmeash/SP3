#include "EntityManager.h"
#include "Scene2D.h"
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
	return false;
}

void EntityManager::Update(const double dElapsedTime)
{
	//iterator
	if (entitylist.size() > 0)
	{
		for (std::vector<CEntity2D*>::iterator it = entitylist.begin(); it != entitylist.end(); ++it)
		{
			CEntity2D* entity = (CEntity2D*)*it;
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
						if (cPhysics.CalculateDistance(entity->vec2WSCoordinate,CPlayer2D::GetInstance()->vec2WSCoordinate) <= 1)
						{
							CPlayer2D::GetInstance()->PlayerDamaged();
							entity->bIsActive = false;
							EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(entity));
						}
						break;
					}
					case CEntity2D::E_BULLET:
					{
						for (std::vector<CEntity2D*>::iterator it2 = CScene2D::GetInstance()->enemyVector.begin(); it2 != CScene2D::GetInstance()->enemyVector.end(); ++it2)
						{
							CEntity2D* enemy = (CEntity2D*)*it2;
							if (!enemy->bIsActive) continue;
							if (cPhysics.CalculateDistance(entity->vec2WSCoordinate, enemy->vec2WSCoordinate) <= 1)
							{
								enemy->bIsActive = false;
								entity->bIsActive = false;
								EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(enemy));
								EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(entity));
							}
						}
						if (!entity->bIsActive) break;
						if (cMap2D->GetMapInfo((int)entity->vec2WSCoordinate.y, (int)entity->vec2WSCoordinate.x) >= 100)
						{
							entity->bIsActive = false;
							EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(entity));
						}
						break;
					}
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
