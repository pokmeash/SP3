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
						glm::i32vec2 temp;
						temp.x = (int)entity->vec2WSCoordinate.x;
						temp.y = (int)entity->vec2WSCoordinate.y;
						if (temp.x < 0 || temp.x > CSettings::GetInstance()->NUM_TILES_XAXIS || 
							temp.y < 0 || temp.y > CSettings::GetInstance()->NUM_TILES_YAXIS) {
								entity->bIsActive = false;
								std::cout << "despawn bullet\n";
								break;
						}

						glm::vec2 vel = entity->vec2Velocity;
						if (cMap2D->GetMapInfo((int)temp.y + 1, (int)temp.x) >= 100 && vel.y > 0)
						{
							entity->vec2Velocity.y *= -1;
						}
						if (cMap2D->GetMapInfo((int)temp.y - 1, (int)temp.x) >= 100 && vel.y < 0)
						{
							entity->vec2Velocity.y *= -1;
						}
						if (cMap2D->GetMapInfo((int)temp.y, (int)temp.x + 1) >= 100 && vel.x > 0)
						{
							entity->vec2Velocity.x *= -1;
						}
						if (cMap2D->GetMapInfo((int)temp.y, (int)temp.x - 1) >= 100 && vel.x < 0)
						{
							entity->vec2Velocity.x *= -1;
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
