#include "EntityFactory.h"
#include "EventControl/EventHandler.h"
#include "EntityManager.h"
#include "Projectile/PortalManager.h"

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

std::vector<Beam*> EntityFactory::ProduceBeam(glm::vec2 pos, glm::vec2 dir, CEntity2D::ENTITY_TYPE type)
{
	std::vector<Beam*> beams;
	for (unsigned i = 0; i < EntityManager::GetInstance()->entitylist.size(); ++i) {
		CEntity2D* entity = EntityManager::GetInstance()->entitylist[i];
		if (entity->bIsActive) continue;
		if (!dynamic_cast<Beam*>(entity)) continue;
		beams.push_back((Beam*)entity);
		break;
	}
	for (unsigned i = 0; i < beams.size(); ++i) {
		Beam* beam = beams[i];
		beam->vec2WSCoordinate = pos;
		beam->vec2Velocity = dir;
		beam->type = type;
		beam->timer = 0.1f;
		beam->bIsActive = true;
		pos += dir;
		EventHandler::GetInstance()->CallThenDelete(new Entity2DSpawnEvent(beam));
		if (PortalManager::GetInstance()->getPortal(pos) && PortalManager::GetInstance()->getPortal(pos)->getDestination()) {
			Portal* portal = PortalManager::GetInstance()->getPortal(pos)->getDestination();
			pos = portal->vec2WSCoordinate + dir;
			while (glm::length(pos - portal->vec2WSCoordinate) <= 1.f) {
				pos += dir;
			}
		}
	}
	while (beams.size() < 10) {
		Beam* temp = new Beam();
		temp->Init();
		EntityManager::GetInstance()->entitylist.push_back(temp);
		temp->vec2WSCoordinate = pos;
		temp->vec2Velocity = dir;
		temp->type = type;
		temp->bIsActive = true;
		beams.push_back(temp);
		EventHandler::GetInstance()->CallThenDelete(new Entity2DSpawnEvent(temp));
	}
	return beams;
}
