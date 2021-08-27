/**
 CSpaceGoop
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "SpaceGoop.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"
// Include Mesh Builder
#include "Primitives/MeshBuilder.h"

// Include GLEW
#include <GL/glew.h>

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "../FloorManager.h"
// Include math.h
#include <math.h>

#include "../Player2D.h"

#include "../EntityManager.h"

#include "EventControl/EventHandler.h"

#include "EventControl/Entity2DMoveEvent.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CSpaceGoop::CSpaceGoop(void)
{
	transform = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first
	rotation = 0.f;
	scale = glm::vec3(1, 1, 1);
	// Initialise vecIndex
	i32vec2Index = glm::i32vec2(0);
	vec2WSCoordinate = glm::vec2(0);
	// Initialise vecNumMicroSteps
	i32vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	i32vec2Destination = glm::i32vec2(0, 0);	// Initialise the iDestination
	i32vec2Direction = glm::i32vec2(0, 0);		// Initialise the iDirection


}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CSpaceGoop::~CSpaceGoop(void)
{
	// Delete the quadMesh
	if (mesh)
	{
		delete mesh;
		mesh = NULL;
	}
	if (animatedSprites)
	{
		delete animatedSprites;
		animatedSprites = NULL;
	}

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/**
  @brief Initialise this instance
  */
bool CSpaceGoop::Init(void)
{
	
	CEnemy2D::Init();
	std::cout << "Initing SpaceGoop\n";
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(1001, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 96);

	// Set the start position of the Player to iRow and iCol
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	vec2WSCoordinate = glm::vec2(uiCol + 0.5f, uiRow);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	// Load the enemy2D texture
	LoadTexture("Image/enemy1.png");

	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(3, 3, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idle", 3, 5);
	animatedSprites->AddAnimation("right", 0, 2);
	animatedSprites->AddAnimation("left", 6, 8);

	//Attributes
	setHP(5);
	setDmg(1);

	return true;
}

/**
 @brief Update this instance
 */
void CSpaceGoop::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;

	vec2WSOldCoordinates = vec2WSCoordinate;
	switch (sCurrentFSM)
	{
	case IDLE:
		//Means that each state changes every 2 seconds
		if (iFSMCounter > iMaxFSMCounter)
		{
			sCurrentFSM = MOVELEFT;
			iFSMCounter = 0;
		}
		iFSMCounter++;
		animatedSprites->PlayAnimation("idle", -1, 1.0f);
		break;
	case MELEEATTACK:
		PathFinding();
		UpdateDirection();
		UpdatePosition();

		if (cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) > 5.0f)
		{
			sCurrentFSM = MOVELEFT;
			iFSMCounter = 0;
			currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
		}
		iFSMCounter++;
		break;
	case MOVELEFT:
		//movementLEFT
		if (vec2WSCoordinate.x >= 0)
		{
			vec2WSCoordinate.x -= 1.f / cSettings->NUM_STEPS_PER_TILE_XAXIS;
			animatedSprites->PlayAnimation("left", -1, 1.0f);
		}
		cSettings->ConvertFloatToIndexSpace(cSettings->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);


		Constraint(LEFT);

		//Constraints
		if (CheckPosition(LEFT) == false)
		{
			vec2WSCoordinate.x += 1.f / cSettings->NUM_STEPS_PER_TILE_XAXIS;
			cSettings->ConvertFloatToIndexSpace(cSettings->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
			sCurrentFSM = MOVERIGHT;
		}
		vec2Velocity = vec2WSCoordinate - vec2WSOldCoordinates;
		if (vec2WSOldCoordinates != vec2WSCoordinate) {
			if (EventHandler::GetInstance()->CallDeleteIsCancelled(new Entity2DMoveEvent(this, vec2WSCoordinate, vec2WSOldCoordinates))) {
				vec2WSCoordinate = vec2WSOldCoordinates;
				CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
				CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->y, vec2WSCoordinate.y, &i32vec2Index.y, &i32vec2NumMicroSteps.y);
			}
		}
		if (cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) < 5.0f)
		{
			sCurrentFSM = MELEEATTACK;
		}
	break;

	case MOVERIGHT:
		if (vec2WSCoordinate.x < cSettings->NUM_TILES_XAXIS)
		{
			vec2WSCoordinate.x += 1.f / cSettings->NUM_STEPS_PER_TILE_XAXIS;
			animatedSprites->PlayAnimation("right", -1, 1.0f);
		}
		cSettings->ConvertFloatToIndexSpace(cSettings->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
		

		// Constraint the player's position within the screen boundary
		Constraint(RIGHT);

		vec2WSCoordinate.x = cSettings->ConvertIndexToWSSpace(cSettings->x, i32vec2Index.x, i32vec2NumMicroSteps.x);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(RIGHT) == false)
		{
			FlipHorizontalDirection();
			i32vec2NumMicroSteps.x = 0;
			vec2WSCoordinate.x = cSettings->ConvertIndexToWSSpace(cSettings->x, i32vec2Index.x, i32vec2NumMicroSteps.x);
			sCurrentFSM = MOVELEFT;
		}
		vec2Velocity = vec2WSCoordinate - vec2WSOldCoordinates;
		if (vec2WSOldCoordinates != vec2WSCoordinate) {
			if (EventHandler::GetInstance()->CallDeleteIsCancelled(new Entity2DMoveEvent(this, vec2WSCoordinate, vec2WSOldCoordinates))) {
				vec2WSCoordinate = vec2WSOldCoordinates;
				CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
				CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->y, vec2WSCoordinate.y, &i32vec2Index.y, &i32vec2NumMicroSteps.y);
			}
		}
		if (cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) < 5.0f)
		{
			sCurrentFSM = MELEEATTACK;
		}
		break;
	default:
		break;
	}
	if (vec2WSOldCoordinates != vec2WSCoordinate) {
		if (EventHandler::GetInstance()->CallDeleteIsCancelled(new Entity2DMoveEvent(this, vec2WSCoordinate, vec2WSOldCoordinates))) {
			vec2WSCoordinate = vec2WSOldCoordinates;
		}
	}
	// Update Jump or Fall
	// UpdateJumpFall(dElapsedTime);
	animatedSprites->Update(dElapsedTime);
}
