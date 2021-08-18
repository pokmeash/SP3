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
			if (entity->isactive)
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
							entity->isactive = false;
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
						glm::i32vec2 temp;
						temp.x = (int)entity->EntityVec2Index.x;
						temp.y = (int)entity->EntityVec2Index.y;
						if (cMap2D->GetMapInfo(temp.y, temp.x) >= 100)
						{
							entity->isactive = false;
						}
						for (CEntity2D* cEnemy2D : CScene2D::GetInstance()->enemyVector)
						{
							cout << cEnemy2D->i32vec2Index.x << "," << cEnemy2D->i32vec2Index.y << endl;
							cout << cEnemy2D->vec2WSCoordinate.x << "," << cEnemy2D->vec2WSCoordinate.y << endl;
							if (temp.x >= cEnemy2D->i32vec2Index.x - 1 && temp.x <= cEnemy2D->i32vec2Index.x + 1 && temp.y >= cEnemy2D->i32vec2Index.y - 1 && temp.y <= cEnemy2D->i32vec2Index.y + 1)
							{
								//pass back into enemy
								if (cEnemy2D->enemyhealth <= 0)
								{
									cEnemy2D->isactive = false;
								}
								else
								{
									cEnemy2D->enemyhealth--;
									cout << "hit" << endl;
									cout << cEnemy2D->enemyhealth << endl;
								}
								entity->isactive = false;
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

			if (entity->isactive)
			{
				entity->PreRender();
				entity->Render();
				entity->PostRender();
			}
		}
	}
}
