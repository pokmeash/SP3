/**
 CContagionBoss
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "../Bosses/ContagionBoss.h"

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
CContagionBoss::CContagionBoss(void)
{
	transform = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first

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
CContagionBoss::~CContagionBoss(void)
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

	setHP(10);
	setProjSpeed(0.3);
}

/**
  @brief Initialise this instance
  */
bool CContagionBoss::Init(void)
{
	CEnemy2D::Init();
	std::cout << "Initing SpaceCannon\n";
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(1102, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	vec2WSCoordinate = glm::vec2(uiCol + 0.5f, uiRow);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	// Load the enemy2D texture
	if (LoadTexture("Image/enemy5.png") == false)
	{
		std::cout << "Failed to load enemy2D tile texture" << std::endl;
		return false;
	}

	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 1, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idle", 0, 3);

	srand(time(0));
	beamRand = rand() % 10 + 1;
	

	return true;
}

/**
 @brief Update this instance
 */
void CContagionBoss::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;

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
			bulletTimer += dElapsedTime;
			if (bulletTimer >= 1 && phaseOne == false)
			{
				for (double theta = 0; theta <= 2 * 3.14159; theta += 3.14159 / 3.f) 
				{
					glm::vec2 temp(cos(theta), sin(theta));
					temp = glm::normalize(temp) * .5f;
					EntityFactory::GetInstance()->ProduceBullets(vec2WSCoordinate, temp, glm::vec3(1, 1, 1), E_EBULLET);
					phaseOne = true;
				}
			}

			if (bulletTimer >= 1.8)
			{
				for (double theta1 = 0; theta1 <= 2 * 3.14159; theta1 += 3.14159 / 2.f)
				{
					glm::vec2 temp(cos(theta1 + offset), sin(theta1 + offset));
					temp = glm::normalize(temp) * .5f;
					EntityFactory::GetInstance()->ProduceBullets(vec2WSCoordinate, temp, glm::vec3(1, 1, 1), E_EBULLET);
					cout << beamRand;
					offset += 3.14159 / 8;
				}
				phaseOne = false;
			}

			if (bulletTimer >= 2.2 && phaseTwo == false)
			{
				for (double theta1 = 0; theta1 <= 2 * 3.14159; theta1 += 3.14159 / 12.f)
				{
					glm::vec2 temp(cos(theta1), sin(theta1));
					temp = glm::normalize(temp) * .5f;
					EntityFactory::GetInstance()->ProduceBullets(vec2WSCoordinate, temp, glm::vec3(1, 1, 1), E_EBULLET);
					phaseTwo = true;
				}

				bulletTimer = 0;
				phaseOne = false;
				phaseTwo = false;
			}
		break;
	default:
		break;
	}
}
