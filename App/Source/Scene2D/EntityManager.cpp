#include "EntityManager.h"
#include "Scene2D.h"
#include "../Library/Source/System/MyMath.h"
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
					if (cPhysics.CalculateDistance(entity->vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) <= 1)
					{
						CPlayer2D::GetInstance()->PlayerDamaged();
						entity->bIsActive = false;
					}
					break;
				}
				case CEntity2D::E_BULLET:
				{
					
					if (entity->i32vec2Index.x < 0 || entity->i32vec2Index.x > CSettings::GetInstance()->NUM_TILES_XAXIS ||
						entity->i32vec2Index.y < 0 || entity->i32vec2Index.y > CSettings::GetInstance()->NUM_TILES_YAXIS) {
						entity->bIsActive = false;
						std::cout << "despawn bullet\n";
						break;
					}
					// Ricochet power up physics
					glm::vec2 vel = entity->vec2Velocity;
					if (cMap2D->GetMapInfo(entity->i32vec2Index.y + 1, entity->i32vec2Index.x) >= 100 && vel.y > 0)
					{
						entity->vec2Velocity.y *= -1;
						entity->counter--;
					}
					else if (cMap2D->GetMapInfo(entity->i32vec2Index.y - 1, entity->i32vec2Index.x) >= 100 && vel.y < 0 && entity->i32vec2NumMicroSteps.y <= CSettings::GetInstance()->NUM_STEPS_PER_TILE_YAXIS * 0.5f)
					{
						entity->vec2Velocity.y *= -1;
						entity->counter--;
					}
					if (cMap2D->GetMapInfo(entity->i32vec2Index.y, entity->i32vec2Index.x + 1) >= 100 && vel.x > 0)
					{
						entity->vec2Velocity.x *= -1;
						entity->counter--;
					}
					else if (cMap2D->GetMapInfo(entity->i32vec2Index.y, entity->i32vec2Index.x - 1) >= 100 && vel.x < 0 && entity->i32vec2NumMicroSteps.x <= CSettings::GetInstance()->NUM_STEPS_PER_TILE_XAXIS * 0.5f)
					{
						entity->vec2Velocity.x *= -1;
						entity->counter--;
					}
					for (std::vector<CEntity2D*>::iterator it2 = CScene2D::GetInstance()->enemyVector.begin(); it2 != CScene2D::GetInstance()->enemyVector.end(); ++it2)
					{
						CLivingEntity* enemy = (CLivingEntity*)*it2;
						
						if (!enemy->bIsActive) continue;
						if (cPhysics.CalculateDistance(entity->vec2WSCoordinate, enemy->vec2WSCoordinate) <= 1)
						{
							enemy->setHP(enemy->getHP() - CPlayer2D::GetInstance()->getDmg());
							if (enemy->getHP() <= 0)
							{
								enemy->bIsActive = false;
							}
							entity->bIsActive = false;
						}
					}
					if (entity->counter <= 0)
					{
						entity->bIsActive = false;
					}
					break;
				}
				case CEntity2D::E_FRAGMENT:
					glm::i32vec2 temp;
					temp.x = (int)entity->vec2WSCoordinate.x;
					temp.y = (int)entity->vec2WSCoordinate.y;
					
					entity->vec2Velocity *= 0.8;
					if (entity->i32vec2Index.x < 0 || entity->i32vec2Index.x > CSettings::GetInstance()->NUM_TILES_XAXIS ||
						entity->i32vec2Index.y < 0 || entity->i32vec2Index.y > CSettings::GetInstance()->NUM_TILES_YAXIS) {
						entity->bIsActive = false;
						std::cout << "despawn fragments\n";
						break;
					}
					if (cMap2D->GetMapInfo(temp.y, temp.x) >= 100)
					{
						entity->bIsActive = false;
					}
					for (std::vector<CEntity2D*>::iterator it2 = CScene2D::GetInstance()->enemyVector.begin(); it2 != CScene2D::GetInstance()->enemyVector.end(); ++it2)
					{
						CEntity2D* enemy = (CEntity2D*)*it2;
						if (!enemy->bIsActive) continue;
						if (cPhysics.CalculateDistance(entity->vec2WSCoordinate, enemy->vec2WSCoordinate) <= 1)
						{
							enemy->bIsActive = false;
							entity->bIsActive = false;
						}
					}
					if (entity->vec2Velocity.x >= 0 && entity->vec2Velocity.x <= 0.12)
					{
						entity->bIsActive = false;
					}
					break;
				case CEntity2D::E_GRENADE:
					entity->vec2Velocity *= 0.9;
					
					entity->timer -= dElapsedTime;
					glm::vec2 vel = entity->vec2Velocity;
					// if out of bounds delete
					if (entity->i32vec2Index.x < 0 || entity->i32vec2Index.x > CSettings::GetInstance()->NUM_TILES_XAXIS ||
						entity->i32vec2Index.y < 0 || entity->i32vec2Index.y > CSettings::GetInstance()->NUM_TILES_YAXIS) {
						entity->bIsActive = false;
						std::cout << "despawn grenade\n";
						break;
					}
					if (cMap2D->GetMapInfo(entity->i32vec2Index.y + 1, entity->i32vec2Index.x) >= 100 && vel.y > 0)
					{
						entity->vec2Velocity.y *= -1;
					}
					else if (cMap2D->GetMapInfo(entity->i32vec2Index.y - 1, entity->i32vec2Index.x) >= 100 && vel.y < 0 && entity->i32vec2NumMicroSteps.y <= CSettings::GetInstance()->NUM_STEPS_PER_TILE_YAXIS * 0.5f)
					{
						entity->vec2Velocity.y *= -1;
					}
					if (cMap2D->GetMapInfo(entity->i32vec2Index.y, entity->i32vec2Index.x + 1) >= 100 && vel.x > 0)
					{
						entity->vec2Velocity.x *= -1;
					}
					else if (cMap2D->GetMapInfo(entity->i32vec2Index.y, entity->i32vec2Index.x - 1) >= 100 && vel.x < 0 && entity->i32vec2NumMicroSteps.x <= CSettings::GetInstance()->NUM_STEPS_PER_TILE_XAXIS * 0.5f)
					{
						entity->vec2Velocity.x *= -1;
					}
					if (entity->timer <= 0)
					{
						entity->timer = dElapsedTime;
						entity->bIsActive = false;
						tempPos = entity->vec2WSCoordinate;
						setExplode();
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


