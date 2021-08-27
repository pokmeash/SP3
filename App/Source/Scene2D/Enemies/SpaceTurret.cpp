/**
 CSpaceTurret
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "SpaceTurret.h"

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

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CSpaceTurret::CSpaceTurret(void)
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
CSpaceTurret::~CSpaceTurret(void)
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
bool CSpaceTurret::Init(void)
{
	CEnemy2D::Init();
	std::cout << "Initing SpaceTurret\n";
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(1003, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	vec2WSCoordinate = glm::vec2(uiCol + 0.5f, uiRow);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	// Load the enemy2D texture
	if (LoadTexture("Image/enemy4.png") == false)
	{
		std::cout << "Failed to load enemy2D tile texture" << std::endl;
		return false;
	}

	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idle", 0, 3);

	//Attributes
	setHP(10);
	setDmg(1);

	return true;
}

/**
 @brief Update this instance
 */
void CSpaceTurret::Update(const double dElapsedTime)
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
			sCurrentFSM = SHOOT;
			iFSMCounter = 0;
		}
		iFSMCounter++;
		animatedSprites->PlayAnimation("idle", -1, 1.0f);
		break;
	case SHOOT:
		if (cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) < 10.0f)
		{
			bulletTimer += dElapsedTime;
			glm::vec2 direction = CPlayer2D::GetInstance()->vec2WSCoordinate - vec2WSCoordinate;
			direction = glm::normalize(direction);

			cout << direction.x << ", " << direction.y << endl;

			if (bulletTimer >= 1)
			{
				glm::vec2 temp = direction;
				temp.y = sinf(atan2f(temp.y, temp.x) + 0.1);
				temp.x = cosf(atan2f(temp.y, temp.x) + 0.1);
				temp = glm::normalize(temp) * 0.5f;
				EntityFactory::GetInstance()->ProduceBullets(vec2WSCoordinate, glm::vec2(temp.x, temp.y), glm::vec3(1, 1, 1), E_EBULLET);
				bulletTimer = 0;
			}
		}
	default:
		break;
	}
	animatedSprites->Update(dElapsedTime);
}
