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
				if (entity->type == CEntity2D::E_EBULLET)
				{
					glm::i32vec2 temp;
					temp.x = (int)entity->EntityVec2Index.x;
					temp.y = (int)entity->EntityVec2Index.y;
					if (temp == CPlayer2D::GetInstance()->i32vec2Index)
					{
						CPlayer2D::GetInstance()->PlayerDamaged();
						entity->isactive = false;
					}
				}
				else if (entity->type == CEntity2D::E_SPIKE)
				{
					glm::i32vec2 temp;
					temp.x = (int)entity->EntityVec2Index.x;
					temp.y = (int)entity->EntityVec2Index.y;
					for (CEntity2D* cEnemy2D:CScene2D::GetInstance()->enemyVector)
					{
						if (temp.y <= cEnemy2D->i32vec2Index.y)
						{
							//pass back into enemy
							cEnemy2D->spikecollided = true;
						}
					}
				}
				if (entity->type == CEntity2D::E_BULLET)
				{
					glm::i32vec2 temp;
					temp.x = (int)entity->EntityVec2Index.x;
					temp.y = (int)entity->EntityVec2Index.y;
					cout << temp.x << ", " << temp.y << endl;
					if (cMap2D->GetMapInfo(temp.y, temp.x) >= 100)
					{
						entity->isactive = false;
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
