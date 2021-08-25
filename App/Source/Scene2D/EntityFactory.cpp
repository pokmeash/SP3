#include "EntityFactory.h"
#include "EventControl/EventHandler.h"
#include "EntityManager.h"
#include "Projectile/PortalManager.h"
#include "Scene2D.h"

EntityFactory::EntityFactory()
{}
EntityFactory::~EntityFactory()
{}

Bullet* EntityFactory::ProduceBullets(glm::f32vec2 EntityVec2Index, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, CEntity2D::ENTITY_TYPE type)
{
	Bullet* temp = nullptr;
	for (unsigned i = 0; i < EntityManager::GetInstance()->entitylist.size(); ++i) {
		CEntity2D* entity = EntityManager::GetInstance()->entitylist[i];
		if (entity->bIsActive) continue;
		if (!dynamic_cast<Bullet*>(entity)) continue;
		temp = (Bullet*)entity;
		break;
	}
	if (!temp) {
		temp = new Bullet();
		temp->Init();
		EntityManager::GetInstance()->entitylist.push_back(temp);
	}
	temp->counter = 3;
	temp->vec2WSCoordinate = EntityVec2Index;
	temp->vec2Velocity = EntityVec2Vel;
	temp->scale = EntityVec3Scale;
	temp->rotation = atan2f(EntityVec2Vel.y, EntityVec2Vel.x);
	temp->type = type;
	temp->bIsActive = true;
	
	EventHandler::GetInstance()->CallThenDelete(new Entity2DSpawnEvent(temp));
	return temp;
}

Grenade* EntityFactory::ProduceGrenade(glm::f32vec2 EntityVec2Index, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, CEntity2D::ENTITY_TYPE type)
{
	Grenade* temp = nullptr;
	for (unsigned i = 0; i < EntityManager::GetInstance()->entitylist.size(); ++i) {
		CEntity2D* entity = EntityManager::GetInstance()->entitylist[i];
		if (entity->bIsActive) continue;
		if (!dynamic_cast<Grenade*>(entity)) continue;
		temp = (Grenade*)entity;
		break;
	}
	if (!temp) {
		temp = new Grenade();
		temp->Init();
		EntityManager::GetInstance()->entitylist.push_back(temp);
	}
	temp->timer = 2;
	temp->vec2WSCoordinate = EntityVec2Index;
	temp->vec2Velocity = EntityVec2Vel;
	temp->scale = EntityVec3Scale;
	temp->rotation = 0;
	temp->type = type;
	temp->bIsActive = true;
	EventHandler::GetInstance()->CallThenDelete(new Entity2DSpawnEvent(temp));
	return temp;
}

std::vector<Beam*> EntityFactory::ProduceBeam(glm::vec2 pos, glm::vec2 dir, CEntity2D::ENTITY_TYPE type, unsigned length)
{
	std::vector<Beam*> beams;
	for (unsigned i = 0; i < EntityManager::GetInstance()->entitylist.size(); ++i) {
		CEntity2D* entity = EntityManager::GetInstance()->entitylist[i];
		if (entity->bIsActive) continue;
		if (!dynamic_cast<Beam*>(entity)) continue;
		beams.push_back((Beam*)entity);
		break;
	}
	while (beams.size() < length) {
		Beam* temp = new Beam();
		temp->Init();
		EntityManager::GetInstance()->entitylist.push_back(temp);
		beams.push_back(temp);
	}
	for (unsigned i = 0; i < beams.size(); ++i) {
		Beam* beam = beams[i];
		beam->vec2WSCoordinate = pos;
		beam->vec2Velocity = dir;
		beam->type = type;
		beam->timer = 0.1f;
		beam->bIsActive = true;
		EventHandler::GetInstance()->CallThenDelete(new Entity2DSpawnEvent(beam));
		if (PortalManager::GetInstance()->getPortal(pos) && PortalManager::GetInstance()->getPortal(pos)->getDestination()) {
			Portal* portal = PortalManager::GetInstance()->getPortal(pos)->getDestination();
			glm::vec2 dist = pos - PortalManager::GetInstance()->getPortal(pos)->vec2WSCoordinate;
			pos = dist + portal->vec2WSCoordinate + dir * 0.8f;
			while (glm::length(pos - portal->vec2WSCoordinate) <= .5f) {
				pos += dir * 0.8f;
			}
		}
		glm::i32vec2 index, micro;
		CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::x, pos.x, &index.x, &micro.x);
		CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::y, pos.y, &index.y, &micro.y);
		if (index.x > 0 && index.x < CSettings::GetInstance()->NUM_TILES_XAXIS && index.y > 0 && index.y < CSettings::GetInstance()->NUM_TILES_YAXIS) {
			CFloorManager* cMap2D = CFloorManager::GetInstance();
			if (cMap2D->GetMapInfo(index.y + 1, index.x) >= 100 && dir.y > 0)
			{
				dir.y *= -1;
			} else if (cMap2D->GetMapInfo(index.y - 1, index.x) >= 100 && dir.y < 0 && micro.y <= CSettings::GetInstance()->NUM_STEPS_PER_TILE_YAXIS * 0.5f)
			{
				dir.y *= -1;
			}
			if (cMap2D->GetMapInfo(index.y, index.x + 1) >= 100 && dir.x > 0)
			{
				dir.x *= -1;
			} else if (cMap2D->GetMapInfo(index.y, index.x - 1) >= 100 && dir.x < 0 && micro.x <= CSettings::GetInstance()->NUM_STEPS_PER_TILE_XAXIS * 0.25f)
			{
				dir.x *= -1;
			}
		}
		pos += dir * 0.8f;
		for (unsigned j = 0; j < CScene2D::GetInstance()->enemyVector.size(); ++j) {
			CLivingEntity* enemy = (CLivingEntity*)CScene2D::GetInstance()->enemyVector[j];
			if (!enemy->bIsActive) continue;
			if (glm::length(enemy->vec2WSCoordinate - pos) <= .5f) {
				enemy->addHP(-1);
				if (enemy->getHP() <= 0) {
					enemy->bIsActive = false;
					EventHandler::GetInstance()->CallThenDelete(new Entity2DDespawnEvent(enemy));
				}
			}
		}
	}
	return beams;
}
