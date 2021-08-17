#include "EntityFactory.h"

EntityFactory::EntityFactory()
{}
EntityFactory::~EntityFactory()
{}

Bullet* EntityFactory::ProduceBullets(glm::f32vec2 EntityVec2Index, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, float rotation, CEntity2D::ENTITY_TYPE type)
{
	Bullet* temp = new Bullet;

	temp->EntityVec2Index = EntityVec2Index;
	temp->EntityVec2Vel = EntityVec2Vel;
	temp->EntityVec3Scale = EntityVec3Scale;
	temp->rotation = rotation;
	temp->type = type;
	temp->isactive = true;

	temp->Init();

	return temp;
}

Spike* EntityFactory::ProduceSpikes(float EntityVec2Indexx,float EntityVec2Indexy, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, float rotation, CEntity2D::ENTITY_TYPE type)
{
	Spike* temp = new Spike;

	temp->EntityVec2Index.x = EntityVec2Indexx;
	temp->EntityVec2Index.y = EntityVec2Indexy;
	temp->EntityVec2Vel = EntityVec2Vel;
	temp->EntityVec3Scale = EntityVec3Scale;
	temp->rotation = rotation;
	temp->type = type;
	temp->isactive = true;

	temp->Init();

	return temp;
}

//CEnemy2D* EntityFactory::ProduceEnemy(void)
//{
//	CEnemy2D* temp = new CEnemy2D;
//	temp->SetShader("2DColorShader");
//	return nullptr;
//}

