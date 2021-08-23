/**
 CEnemy2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include shader
#include "RenderControl\shader.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

// Include CEntity2D
#include "../LivingEntity.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CFloorManager;

// Include Settings
#include "GameControl\Settings.h"

// Include Physics2D
#include "../Physics2D.h"

// Game Manager
#include "../GameManager.h"

class CEnemy2D : public CLivingEntity
{
public:
	// Constructor
	CEnemy2D(void);

	// Destructor
	virtual ~CEnemy2D(void);

	// Init
	virtual bool Init(void);

	// Update
	virtual void Update(const double dElapsedTime) = 0;


protected:
	virtual bool InteractWithPlayer(void);
	virtual bool InteractWithEnemy(void);
};