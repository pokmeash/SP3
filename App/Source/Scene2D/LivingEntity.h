/**
 CLivingEntity
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
#include "Primitives/Entity2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CFloorManager;

// Include Settings
#include "GameControl\Settings.h"

// Include Physics2D
#include "Physics2D.h"

// Game Manager
#include "GameManager.h"

class CLivingEntity : public CEntity2D
{
public:
	// Constructor
	CLivingEntity(void);

	// Destructor
	virtual ~CLivingEntity(void);

	// Update
	virtual void Update(const double dElapsedTime) = 0;

	// Set the indices of the enemy2D
	void Seti32vec2Index(const int iIndex_XAxis, const int iIndex_YAxis);

	// Set the number of microsteps of the enemy2D
	void Seti32vec2NumMicroSteps(const int iNumMicroSteps_XAxis, const int iNumMicroSteps_YAxis);

	// Set the UV coordinates of the enemy2D
	void Setvec2UVCoordinates(const float fUVCoordinate_XAxis, const float fUVCoordinate_YAxis);

	// Get the indices of the enemy2D
	glm::i32vec2 Geti32vec2Index(void) const;

	// Get the number of microsteps of the enemy2D
	glm::i32vec2 Geti32vec2NumMicroSteps(void) const;

	// Set the UV coordinates of the enemy2D
	glm::vec2 Getvec2UVCoordinates(void) const;
protected:
	enum FSM
	{
		IDLE = 0,
		SEARCH = 1,
		ATTACK = 2,
		RANGEDATTACK = 3,
		MELEEATTACK = 4,
		RUSHATTACK = 5,
		HEALING = 6,
		DAMAGED = 7,
		NUM_FSM
	};

	// The i32vec2 which stores the indices of the destination for enemy2D in the Map2D
	glm::i32vec2 i32vec2Destination;

	// The i32vec2 which stores the direction for enemy2D movement in the Map2D
	glm::i32vec2 i32vec2Direction;

	CFloorManager* cMap2D;

	// Physics
	CPhysics2D cPhysics2D;

	// Current color
	glm::vec4 currentColor;

	// Current FSM
	FSM sCurrentFSM;

	// FSM counter - count how many frames it has been in this FSM
	int iFSMCounter;

	// Max count in a state
	const int iMaxFSMCounter = 60;

	// Constraint the enemy2D's position within a boundary
	void Constraint(DIRECTION eDirection = LEFT);

	// Check if a position is possible to move into
	bool CheckPosition(DIRECTION eDirection);

	// Update direction
	void UpdateDirection(void);

	// Flip horizontal direction. For patrol use only
	void FlipHorizontalDirection(void);

	// Update position
	void UpdatePosition(void);

	virtual bool InteractWithPlayer(void) { return false; }
	virtual void InteractWithMap(void) {}
};