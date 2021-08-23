/**
 CSpaceSkeleton
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "SpaceSkeleton.h"

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
#include "../MapManager.h"
// Include math.h
#include <math.h>

#include "../Player2D.h"

#include "../EntityManager.h"
/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CSpaceSkeleton::CSpaceSkeleton(void)
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
CSpaceSkeleton::~CSpaceSkeleton(void)
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
bool CSpaceSkeleton::Init(void)
{
	CEnemy2D::Init();
	std::cout << "Initing spacegoop\n";
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(1004, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	vec2WSCoordinate = glm::vec2(uiCol + 0.5f, uiRow);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	// Load the enemy2D texture
	if (LoadTexture("Image/enemy2.png") == false)
	{
		std::cout << "Failed to load enemy2D tile texture" << std::endl;
		return false;
	}

	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(4, 9, cSettings->TILE_WIDTH * 2, cSettings->TILE_HEIGHT * 2);
	animatedSprites->AddAnimation("idle", 9, 17);
	animatedSprites->AddAnimation("right", 27, 35);
	animatedSprites->AddAnimation("up", 0, 8);
	animatedSprites->AddAnimation("down", 18, 26);

	bulletTimer = 0;

	iHealth = 4;
	return true;
}

/**
 @brief Update this instance
 */
void CSpaceSkeleton::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;

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

			//SHOOTING
			bulletTimer += dElapsedTime;
			glm::vec2 direction = CPlayer2D::GetInstance()->vec2WSCoordinate - vec2WSCoordinate;
			direction = glm::normalize(direction);

			cout << direction.x << ", " << direction.y << endl;

			if (bulletTimer >= 2)
			{
				glm::vec2 temp = direction;
				temp.y = sinf(atan2f(temp.y, temp.x) + 0.1);
				temp.x = cosf(atan2f(temp.y, temp.x) + 0.1);
				temp = glm::normalize(temp) * 0.5f;
				EntityManager::GetInstance()->entitylist.push_back(EntityFactory::GetInstance()->ProduceBullets(vec2WSCoordinate, glm::vec2(temp.x, temp.y), glm::vec3(1, 1, 1), 0, E_EBULLET));

				cout << "shoot bullet!";
				bulletTimer = 0;
			}

			//Moving Up and Down
			if (vec2WSCoordinate.y < CPlayer2D::GetInstance()->vec2WSCoordinate.y)
			{
				animatedSprites->PlayAnimation("up", -1, 1.0f);
			}

			else if (vec2WSCoordinate.y > CPlayer2D::GetInstance()->vec2WSCoordinate.y)
			{
				animatedSprites->PlayAnimation("down", -1, 1.0f);
			}

			// Attack
			// Update direction to move towards for attack
			UpdateDirection();

			// Update the Enemy2D's position for attack
			UpdatePosition();
		}

		else
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
			animatedSprites->PlayAnimation("idle", -1, 1.0f);
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

		if (cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) < 5.0f)
		{
			sCurrentFSM = MELEEATTACK;
		}
		break;
	default:
		break;
	}

	// Update Jump or Fall
	// UpdateJumpFall(dElapsedTime);
	animatedSprites->Update(dElapsedTime);
}

/**
 @brief Let enemy2D interact with the player.
 */
bool CSpaceSkeleton::InteractWithPlayer(void)
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