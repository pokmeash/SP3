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
	cMap2D = CMapManager::GetInstance();
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
						glm::i32vec2 temp;
						temp.x = (int)entity->vec2WSCoordinate.x;
						temp.y = (int)entity->vec2WSCoordinate.y;
						if (temp == CPlayer2D::GetInstance()->i32vec2Index)
						{
							CPlayer2D::GetInstance()->PlayerDamaged();
							entity->bIsActive = false;
						}
						break;
					}
					case CEntity2D::E_SPIKE:
					{
						glm::i32vec2 temp;
						temp.x = (int)entity->vec2WSCoordinate.x;
						temp.y = (int)entity->vec2WSCoordinate.y;
						for (CEntity2D* cEnemy2D : CScene2D::GetInstance()->enemyVector)
						{
							if (temp.y <= cEnemy2D->i32vec2Index.y)
							{
								//pass back into enemy
								cEnemy2D->bIsActive = false;
							}
						}
						break;
					}
					case CEntity2D::E_BULLET:
					{
						glm::i32vec2 temp;
						temp.x = (int)entity->vec2WSCoordinate.x;
						temp.y = (int)entity->vec2WSCoordinate.y;
						if (cMap2D->GetMapInfo(temp.y, temp.x) >= 100)
						{
							entity->bIsActive = false;
						}
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
