#include "EntityManager.h"
#include "Scene2D.h"

EntityManager::EntityManager(void) : cMap2D(NULL), cPhysics(NULL)
{
}

EntityManager::~EntityManager(void)
{
	if (cPhysics) {
		delete cPhysics;
		cPhysics = NULL;
	}
}

bool EntityManager::Init(void)
{
	cMap2D = CMapManager::GetInstance();
	cPhysics = new CPhysics2D();
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
						temp.x = (int)entity->EntityVec2Index.x;
						temp.y = (int)entity->EntityVec2Index.y;
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
						temp.x = (int)entity->EntityVec2Index.x;
						temp.y = (int)entity->EntityVec2Index.y;
						for (CEntity2D* cEnemy2D : CScene2D::GetInstance()->enemyVector)
						{
							if (temp.y <= cEnemy2D->i32vec2Index.y)
							{
								//pass back into enemy
								cEnemy2D->spikecollided = true;
							}
						}
						break;
					}
					case CEntity2D::E_BULLET:
					{
						if (cMap2D->GetMapInfo((int)entity->EntityVec2Index.y, (int)entity->EntityVec2Index.x) >= 100)
						{
							entity->bIsActive = false;
						}
						for (CEntity2D* cEnemy2D : CScene2D::GetInstance()->enemyVector)
						{
							if (cPhysics->CalculateDistance(cEnemy2D->vec2WSCoordinate, entity->EntityVec2Index) <= 1.f)
							{
								//pass back into enemy
								if (cEnemy2D->enemyhealth <= 0)
								{
									cEnemy2D->bIsActive = false;
								}
								else
								{
									cEnemy2D->enemyhealth--;
									cout << "hit" << endl;
									cout << cEnemy2D->enemyhealth << endl;
								}
								entity->bIsActive = false;
							}
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
