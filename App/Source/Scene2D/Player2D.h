/**
 CPlayer2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include Singleton template
#include "DesignPatterns\SingletonTemplate.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CEntity2D
#include "LivingEntity.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CMapManager;

// Include Keyboard controller
#include "Inputs\KeyboardController.h"
#include "Inputs\MouseController.h"

// Include Physics2D
#include "Physics2D.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

// Include InventoryManager
#include "InventoryManager.h"

// Include SoundController
#include "..\SoundController\SoundController.h"

#include "EntityManager.h"

extern bool wings;
class CPlayer2D : public CSingletonTemplate<CPlayer2D>, public CLivingEntity
{
	friend CSingletonTemplate<CPlayer2D>;
public:

	// Init
	bool Init(void);

	// Reset
	bool Reset(void);

	// Update
	void Update(const double dElapsedTime);

	virtual void Render();

	void PlayerDamaged();

protected:

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;
	CMouseController* cMouseController;

	// Current color
	glm::vec4 currentColor;

	// InventoryManager
	CInventoryManager* cInventoryManager;
	// InventoryItem
	CInventoryItem* cInventoryItem;

	// Count the number of jumps
	int jumpCount;

	// Handler to the CSoundController
	CSoundController* cSoundController;

	EntityManager* cEntityManager;

	EntityFactory* cEntityFactory;

	// Constructor
	CPlayer2D(void);

	// Destructor
	virtual ~CPlayer2D(void);

	// Let player interact with the map
	virtual void InteractWithMap(void);

	// Update the health and lives
	void UpdateHealthLives(void);

	bool swap;
	int dirx; // -1 = left +1 = right
	int diry; // -1 = down +1 = up
};

