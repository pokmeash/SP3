#include "EntityManager.h"
#include "Scene2D.h"

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
				switch (entity->type)
				{
					case CEntity2D::E_EBULLET:
					{
						if (cPhysics.CalculateDistance(entity->vec2WSCoordinate,CPlayer2D::GetInstance()->vec2WSCoordinate) <= 1)
						{
							CPlayer2D::GetInstance()->PlayerDamaged();
							entity->bIsActive = false;
						}
						break;
					}
					case CEntity2D::E_BULLET:
					{
						for (std::vector<CEntity2D*>::iterator it2 = CScene2D::GetInstance()->enemyVector.begin(); it2 != CScene2D::GetInstance()->enemyVector.end(); ++it2)
						{
							CEntity2D* enemy = (CEntity2D*)*it2;
							if (cPhysics.CalculateDistance(entity->vec2WSCoordinate, enemy->vec2WSCoordinate) <= 1)
							{
								enemy->bIsActive = false;
								entity->bIsActive = false;
							}
						}
						if (cMap2D->GetMapInfo((int)entity->vec2WSCoordinate.y, (int)entity->vec2WSCoordinate.x) >= 100)
						{
							entity->bIsActive = false;
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
