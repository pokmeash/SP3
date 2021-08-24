#include "EntityFactory.h"
#include "EventControl/EventHandler.h"
#include "EntityManager.h"

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

Beam* EntityFactory::ProduceBeam(glm::vec2 pos, glm::vec2 dir, CEntity2D::ENTITY_TYPE type)
{
	Beam* temp = nullptr;
	for (unsigned i = 0; i < EntityManager::GetInstance()->entitylist.size(); ++i) {
		CEntity2D* entity = EntityManager::GetInstance()->entitylist[i];
		if (entity->bIsActive) continue;
		if (!dynamic_cast<Beam*>(entity)) continue;
		temp = (Beam*)entity;
		break;
	}
	if (!temp) {
		temp = new Beam();
		temp->Init();
		EntityManager::GetInstance()->entitylist.push_back(temp);
	}
	temp->vec2WSCoordinate = pos;
	temp->vec2Velocity = dir;
	temp->type = type;
	temp->bIsActive = true;
	EventHandler::GetInstance()->CallThenDelete(new Entity2DSpawnEvent(temp));
	return temp;
}
