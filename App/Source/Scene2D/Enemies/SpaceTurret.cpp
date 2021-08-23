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
/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CSpaceTurret::CSpaceTurret(void)
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
	std::cout << "Initing spacegoop\n";
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

	bulletTimer = 0;

	iHealth = 4;
	return true;
}

/**
 @brief Update this instance
 */
void CSpaceTurret::Update(const double dElapsedTime)
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
	case MELEEATTACK:
		if (cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) < 10.0f)
		{
			// Calculate a path to the player
			auto path = cMap2D->PathFind(i32vec2Index,
				CPlayer2D::GetInstance()->i32vec2Index,
				heuristic::euclidean,
				10);

			// Calculate new destination
			bool bFirstPosition = true;
			for (const auto& coord : path)
			{
				//std::cout << coord.x << "," << coord.y << "\n";
				if (bFirstPosition == true)
				{
					// Set a destination
					i32vec2Destination = coord;
					// Calculate the direction between enemy2D and this destination
					i32vec2Direction = i32vec2Destination - i32vec2Index;
					bFirstPosition = false;
				}
				else
				{
					if ((coord - i32vec2Destination) == i32vec2Direction)
					{
						// Set a destination
						i32vec2Destination = coord;
					}
					else
						break;
				}
			}
			UpdateDirection();
			UpdatePosition();
		}
		else
		{
			if (iFSMCounter > iMaxFSMCounter)
			{
				sCurrentFSM = SEARCH;
				iFSMCounter = 0;
			}
			iFSMCounter++;
		}
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
				EntityManager::GetInstance()->entitylist.push_back(EntityFactory::GetInstance()->ProduceBullets(vec2WSCoordinate, glm::vec2(temp.x, temp.y), glm::vec3(1, 1, 1), 0, E_EBULLET));

				cout << "shoot bullet!";
				bulletTimer = 0;
			}
		}
		break;
	default:
		break;
	}
	animatedSprites->Update(dElapsedTime);
}

/**
 @brief Let enemy2D interact with the player.
 */
bool CSpaceTurret::InteractWithPlayer(void)
{
	glm::i32vec2 i32vec2PlayerPos = CPlayer2D::GetInstance()->i32vec2Index;
	
	// Check if the enemy2D is within 1.5 indices of the player2D
	if (((i32vec2Index.x >= i32vec2PlayerPos.x - 0.5) && 
		(i32vec2Index.x <= i32vec2PlayerPos.x + 0.5))
		&& 
		((i32vec2Index.y >= i32vec2PlayerPos.y - 0.5) &&
		(i32vec2Index.y <= i32vec2PlayerPos.y + 0.5)))
	{
		CPlayer2D::GetInstance()->PlayerDamaged();
		// Since the player has been caught, then reset the FSM
		sCurrentFSM = IDLE;
		iFSMCounter = 0;
		return true;
	}
	return false;
}