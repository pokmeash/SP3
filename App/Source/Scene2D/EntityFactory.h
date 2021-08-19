#pragma once
#include "Primitives/Entity2D.h"
#include "DesignPatterns/SingletonTemplate.h"


//Projectile
<<<<<<< HEAD
#include"Bullet.h"
#include"Spike.h"
=======
#include "./Projectile/Bullet.h"
#include "./Projectile/Spike.h"

//Enemy
#include "./Enemies/Enemy2D.h"

>>>>>>> main
//Powerup
#include "./PowerUps/DoubleShot.h"

class EntityFactory : public CSingletonTemplate<EntityFactory>
{
	friend CSingletonTemplate<EntityFactory>;
public:
	// Constructor
	EntityFactory(void);

	// Destructor
	~EntityFactory(void);

	//Projectiles
	Bullet* ProduceBullets(glm::f32vec2 EntityVec2Index, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale,float rotation,CEntity2D::ENTITY_TYPE type);

	Spike* ProduceSpikes(float EntityVec2Indexx, float EntityVec2Indexy, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, float rotation, CEntity2D::ENTITY_TYPE type);

	//Powerups
	DoubleShot* ProduceDoubleShot(glm::f32vec2 EntityVec2Index, glm::f32vec2 EntityVec2Vel, glm::vec3 EntityVec3Scale, float rotation, CEntity2D::ENTITY_TYPE type);


};

