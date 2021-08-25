#pragma once
#include "Primitives/Entity2D.h"
#include "DesignPatterns/SingletonTemplate.h"


//Projectile
#include "./Projectile/Bullet.h"
#include "./Projectile/Beam.h"
#include "./Projectile/Spike.h"
#include "./Projectile/Grenade.h"
//Enemy
#include "./Enemies/Enemy2D.h"


class EntityFactory : public CSingletonTemplate<EntityFactory>
{
	friend CSingletonTemplate<EntityFactory>;
public:
	// Constructor
	EntityFactory(void);

	// Destructor
	~EntityFactory(void);

	//Projectiles
	Bullet* ProduceBullets(glm::f32vec2 EntityVec2Index, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, CEntity2D::ENTITY_TYPE type);

	Grenade* ProduceGrenade(glm::f32vec2 EntityVec2Index, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, CEntity2D::ENTITY_TYPE type);

	std::vector<Beam*> ProduceBeam(glm::vec2 pos, glm::vec2 dir, CEntity2D::ENTITY_TYPE type);
};

