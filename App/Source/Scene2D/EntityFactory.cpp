#include "EntityFactory.h"

EntityFactory::EntityFactory()
{}
EntityFactory::~EntityFactory()
{}

Bullet* EntityFactory::ProduceBullets(glm::f32vec2 EntityVec2Index, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, float rotation, CEntity2D::ENTITY_TYPE type)
{
	Bullet* temp = new Bullet;

	temp->vec2WSCoordinate = EntityVec2Index;
	temp->vec2Velocity = EntityVec2Vel;
	temp->scale = EntityVec3Scale;
	temp->rotation = rotation;
	temp->type = type;
	temp->bIsActive = true;

	temp->Init();

	return temp;
}

Spike* EntityFactory::ProduceSpikes(float EntityVec2Indexx,float EntityVec2Indexy, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, float rotation, CEntity2D::ENTITY_TYPE type)
{
	Spike* temp = new Spike;

	temp->vec2WSCoordinate.x = EntityVec2Indexx;
	temp->vec2WSCoordinate.y = EntityVec2Indexy;
	temp->vec2Velocity = EntityVec2Vel;
	temp->scale = EntityVec3Scale;
	temp->rotation = rotation;
	temp->type = type;
	temp->bIsActive = true;

	temp->Init();

	return temp;
}

DoubleShot* EntityFactory::ProduceDoubleShot(glm::f32vec2 EntityVec2Index, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, float rotation, CEntity2D::ENTITY_TYPE type)
{
	DoubleShot* temp = new DoubleShot;

	temp->vec2WSCoordinate = EntityVec2Index;
	temp->vec2Velocity = EntityVec2Vel;
	temp->scale = EntityVec3Scale;
	temp->rotation = rotation;
	temp->type = type;
	temp->bIsActive = true;

	temp->Init();

	return temp;
}
