#pragma once

#include"Bullet.h"
#include"Spike.h"
#include"Primitives/Entity2D.h"
#include"DesignPatterns/SingletonTemplate.h"

class EntityFactory : public CSingletonTemplate<EntityFactory>
{
	friend CSingletonTemplate<EntityFactory>;
public:
	// Constructor
	EntityFactory(void);

	// Destructor
	~EntityFactory(void);

	Bullet* ProduceBullets(glm::f32vec2 EntityVec2Index, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale,float rotation,CEntity2D::ENTITY_TYPE type);

	Spike* ProduceSpikes(float EntityVec2Indexx, float EntityVec2Indexy, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, float rotation, CEntity2D::ENTITY_TYPE type);
	

};

