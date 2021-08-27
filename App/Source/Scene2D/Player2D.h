/**
 CPlayer2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include Singleton template
#include "DesignPatterns/SingletonTemplate.h"

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

#include "BaseAttribute.h"


class CPlayer2D : public CSingletonTemplate<CPlayer2D>, public CLivingEntity
{
	friend CSingletonTemplate<CPlayer2D>;

public:
	enum CHARACTERS {
		Male = 0,
		Female,
		TOTAL
	};

	CHARACTERS charType;
	// Init
	bool Init(void);

	// Reset
	bool Reset(void);

	// Update
	void Update(const double dElapsedTime);

	virtual void Render();

	void PlayerDamaged(int amtOfDmg);

	float RNG(float min, float max);

	bool getiFramesState();
	void addRicochetTimes(int times);

	int getRicochetTimes();

	void SelectCharacter(CHARACTERS);

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

	// Constructor
	CPlayer2D(void);

	// Destructor
	virtual ~CPlayer2D(void);

	// Let player interact with the map
	virtual void InteractWithMap(void);

	// Update the health and lives
	void UpdateHealthLives(void);

private:
	//iFrames
	bool iFrames = false;
	bool iframesState = false;
	float iframesTimer = 0.3;
	float iframesDuration = 3;
	int numberOfRicochet = 1;
	
};
