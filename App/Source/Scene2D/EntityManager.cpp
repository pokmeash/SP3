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
						if (temp.x < 0 || temp.x >= CSettings::GetInstance()->NUM_TILES_XAXIS ||
							temp.y < 0 || temp.y >= CSettings::GetInstance()->NUM_TILES_YAXIS)
						{
							entity->isactive = false;
							std::cout << "deactivate " << entity << std::endl;
							break;
						}
						//std::cout << "checking " << entity << std::endl;
						glm::vec2 vel = entity->EntityVec2Vel;
						if (cMap2D->GetMapInfo((int)temp.y + 1, (int)temp.x) >= 100 && vel.y > 0)
						{
							entity->EntityVec2Vel.y *= -1;
						}
						if (cMap2D->GetMapInfo((int)temp.y - 1, (int)temp.x) >= 100 && vel.y < 0)
						{
							entity->EntityVec2Vel.y *= -1;
						}
						if (cMap2D->GetMapInfo((int)temp.y, (int)temp.x + 1) >= 100 && vel.x > 0)
						{
							entity->EntityVec2Vel.x *= -1;
						}
						if (cMap2D->GetMapInfo((int)temp.y, (int)temp.x - 1) >= 100 && vel.x < 0)
						{
							entity->EntityVec2Vel.x *= -1;
						}
						/*if (entity->EntityVec2Vel.y > 0 ||
							entity->EntityVec2Vel.y < 0)
						{
							entity->EntityVec2Vel.y *= -1;
						}*/
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
