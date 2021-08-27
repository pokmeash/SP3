/*
 CSpaceFly
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Boss2D.h"

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
#include "BossTimeControl.h"
#include <time.h>
/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CBoss2D::CBoss2D(void)
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

	setHP(20);
	setDmg(5);
	setProjSpeed(0.5f);
}

CBoss2D::~CBoss2D(void)
{
	cMap2D = NULL;
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
	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/**
  @brief Initialise this instance
  */

bool CBoss2D::Init(void)
{
	CEnemy2D::Init();
	std::cout << "Initing Worm\n";
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(1101, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	vec2WSCoordinate = glm::vec2(uiCol + 0.5f, uiRow);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	// Load the enemy2D texture
	LoadTexture("Image/worm.png");

	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(6, 8, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	scale = glm::vec3(2, 2, 2);
	animatedSprites->AddAnimation("idle", 0, 7);
	animatedSprites->AddAnimation("right", 8, 15);
	animatedSprites->AddAnimation("left", 16, 23);
	animatedSprites->AddAnimation("shoot", 24, 27);
	animatedSprites->AddAnimation("heal", 32, 37);
	animatedSprites->AddAnimation("dig", 40, 42);

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	temphp = 0;
	offset = 0;
	dirx = 0;
	diry = 0;

	return true;
}

/**
 @brief Update this instance
 */

void CBoss2D::Update(const double dElapsedTime)
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
			sCurrentFSM = ATTACK;
			iFSMCounter = 0;
		}
		iFSMCounter++;
		animatedSprites->PlayAnimation("idle", -1, 1.0f);
		cout << "IDLE" << endl;
		break;
	case ATTACK:
		if (iFSMCounter > iMaxFSMCounter)
		{
			if (getHP() <= 10)
			{
				sCurrentFSM = HEALING;
				temphp = getHP();
			}
			else if (cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) >= 20.0f)
			{
				sCurrentFSM = HEALING;
				temphp = getHP();
			}
			else if (cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) >= 10.0f && cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) < 20.0f)
			{
				srand(time(NULL));
				int random = rand()%2 + 1;
				if (random == 1)
				{
					sCurrentFSM = SHOOT;
				}
				else if (random == 2)
				{
					sCurrentFSM = RANGEDATTACK;
				}
			}
			else if (cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) >= 5.0f && cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) < 10.0f)
			{
				sCurrentFSM = SEARCH;
			}
			else if (cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) < 5.0f)
			{
				srand(time(NULL));
				int random = rand() % 2 + 1;
				
				if (random == 1)
				{
					if (CPlayer2D::GetInstance()->vec2WSCoordinate.x < vec2WSCoordinate.x - 3)
					{
						dirx = -1;
						diry = 0;
					}
					else if (CPlayer2D::GetInstance()->vec2WSCoordinate.x > vec2WSCoordinate.x + 3)
					{
						dirx = 1;
						diry = 0;
					}
					else if (CPlayer2D::GetInstance()->vec2WSCoordinate.y < vec2WSCoordinate.y)
					{
						dirx = 0;
						diry = -1;
					}
					else if (CPlayer2D::GetInstance()->vec2WSCoordinate.y > vec2WSCoordinate.y)
					{
						dirx = 0;
						diry = 1;
					}
					sCurrentFSM = MELEEATTACK;
				}
				else if (random == 2)
				{
					rushdirection = CPlayer2D::GetInstance()->vec2WSCoordinate - vec2WSCoordinate;
					sCurrentFSM = RUSHATTACK;
				}
			}
			iFSMCounter = 0;

		}
		iFSMCounter++;
		animatedSprites->PlayAnimation("dig", -1, 1.0f);
		cout << "ATTACK" << endl;
		break;
	case HEALING:
		if (iFSMCounter > iMaxFSMCounter)
		{
			setHP(getHP() + 1);
			iFSMCounter = 0;
			if (getHP() >= 15 || temphp - 3 == getHP())
			{
				sCurrentFSM = ATTACK;
			}
		}
		iFSMCounter++;
		animatedSprites->PlayAnimation("heal", -1, 1.0f);
		cout << "HEAL" << endl;
		break;
	case SEARCH:
		if (cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) >= 5.0f && cPhysics2D.CalculateDistance(vec2WSCoordinate, CPlayer2D::GetInstance()->vec2WSCoordinate) < 10.0f)
		{
			PathFinding();
			UpdateDirection();
			UpdatePosition();
		}
		else
		{
			sCurrentFSM = ATTACK;
		}
		animatedSprites->PlayAnimation("dig", -1, 1.0f);
		cout << "SEARCH" << endl;
		break;
	case SHOOT:
		bulletTimer += dElapsedTime;
		glm::vec2 direction = CPlayer2D::GetInstance()->vec2WSCoordinate - vec2WSCoordinate;
		direction = glm::normalize(direction);

		if (bulletTimer >= 1)
		{
			glm::vec2 temp = direction;
			temp.y = sinf(atan2f(temp.y, temp.x) + 0.1);
			temp.x = cosf(atan2f(temp.y, temp.x) + 0.1);
			temp = glm::normalize(temp) * getProjSpeed();
			EntityFactory::GetInstance()->ProduceBullets(vec2WSCoordinate, glm::vec2(temp.x, temp.y), glm::vec3(1, 1, 1), E_EBULLET);
			bulletTimer = 0;
			sCurrentFSM = ATTACK;
			if (!CBossTimeControl::GetInstance()->isListening())
				CBossTimeControl::GetInstance()->setListening(true);
		}
		animatedSprites->PlayAnimation("shoot", -1, 1.0f);
		cout << "SHOOT" << endl;
		break;
	case RANGEDATTACK:
		bulletTimer += dElapsedTime;
		if (offset > 3.14159)
		{
			offset = 0;
			sCurrentFSM = ATTACK;
		}
		if (bulletTimer >= 0.1)
		{
			for (double theta = 0; theta <= 2 * 3.14159; theta += 3.14159 / 2.f)
			{
				glm::vec2 temp(cos(theta + offset), sin(theta + offset));
				temp = glm::normalize(temp) * projectileSpeed;
				EntityFactory::GetInstance()->ProduceBullets(vec2WSCoordinate, temp, glm::vec3(1, 1, 1), E_EBULLET);
			}
			bulletTimer = 0;
			offset += 3.14159 / 16.f;
		}
		animatedSprites->PlayAnimation("shoot", -1, 1.0f);
		cout << "RANGEDATTACK" << endl;
		break;
	case MELEEATTACK:
		if (dirx == -1)
		{
			// Calculate the new position to the left
			if (vec2WSCoordinate.x >= 0)
			{
				vec2WSCoordinate.x -= 2.f / cSettings->NUM_STEPS_PER_TILE_XAXIS;
			}
			cSettings->ConvertFloatToIndexSpace(cSettings->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);

			// Constraint the player's position within the screen boundary
			Constraint(LEFT);

			// If the new position is not feasible, then revert to old position
			if (CheckPosition(LEFT) == false)
			{
				vec2WSCoordinate.x += 2.f / cSettings->NUM_STEPS_PER_TILE_XAXIS;
				cSettings->ConvertFloatToIndexSpace(cSettings->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
				sCurrentFSM = ATTACK;
			}
			animatedSprites->PlayAnimation("left", -1, 1.0f);
		}
		else if (dirx == 1)
		{
			// Calculate the new position to the right
			if (vec2WSCoordinate.x < cSettings->NUM_TILES_XAXIS)
			{
				vec2WSCoordinate.x += 2.f / cSettings->NUM_STEPS_PER_TILE_XAXIS;
			}

			cSettings->ConvertFloatToIndexSpace(cSettings->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);

			// Constraint the player's position within the screen boundary
			Constraint(RIGHT);

			vec2WSCoordinate.x = cSettings->ConvertIndexToWSSpace(cSettings->x, i32vec2Index.x, i32vec2NumMicroSteps.x);

			// If the new position is not feasible, then revert to old position
			if (CheckPosition(RIGHT) == false)
			{
				i32vec2NumMicroSteps.x = 0;
				vec2WSCoordinate.x = cSettings->ConvertIndexToWSSpace(cSettings->x, i32vec2Index.x, i32vec2NumMicroSteps.x);
				sCurrentFSM = ATTACK;
			}
			animatedSprites->PlayAnimation("right", -1, 1.0f);
		}
		if (diry == 1)
		{
			// Calculate the new position up
			if (vec2WSCoordinate.y < cSettings->NUM_TILES_YAXIS)
			{
				vec2WSCoordinate.y += 2.f / cSettings->NUM_STEPS_PER_TILE_YAXIS;
			}
			cSettings->ConvertFloatToIndexSpace(cSettings->y, vec2WSCoordinate.y, &i32vec2Index.y, &i32vec2NumMicroSteps.y);

			// Constraint the player's position within the screen boundary

			Constraint(UP);
			vec2WSCoordinate.y = cSettings->ConvertIndexToWSSpace(cSettings->y, i32vec2Index.y, i32vec2NumMicroSteps.y);
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(UP) == false)
			{
				i32vec2NumMicroSteps.y = 0;
				vec2WSCoordinate.y = cSettings->ConvertIndexToWSSpace(cSettings->y, i32vec2Index.y, i32vec2NumMicroSteps.y);
				sCurrentFSM = ATTACK;
			}
			animatedSprites->PlayAnimation("idle", -1, 1.0f);

		}
		else if (diry == -1)
		{
			// Calculate the new position down
			if (vec2WSCoordinate.y >= 0)
			{
				vec2WSCoordinate.y -= 2.f / cSettings->NUM_STEPS_PER_TILE_YAXIS;
			}
			cSettings->ConvertFloatToIndexSpace(cSettings->y, vec2WSCoordinate.y, &i32vec2Index.y, &i32vec2NumMicroSteps.y);
			// Constraint the player's position within the screen boundary
			Constraint(DOWN);
			vec2WSCoordinate.y = cSettings->ConvertIndexToWSSpace(cSettings->y, i32vec2Index.y, i32vec2NumMicroSteps.y);
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(DOWN) == false)
			{
				i32vec2Index.y++;
				i32vec2NumMicroSteps.y = 0;
				vec2WSCoordinate.y = cSettings->ConvertIndexToWSSpace(cSettings->y, i32vec2Index.y, i32vec2NumMicroSteps.y);
				sCurrentFSM = ATTACK;
			}
			animatedSprites->PlayAnimation("idle", -1, 1.0f);
		}
		cout << "MELEEATTACK" << endl;
		break;
	case RUSHATTACK:
		if (wallcounter > 2)
		{
			wallcounter = 0;
			sCurrentFSM = ATTACK;
		}
		if (vec2WSCoordinate.y < cSettings->NUM_TILES_YAXIS && vec2WSCoordinate.y >= 0 && vec2WSCoordinate.x < cSettings->NUM_TILES_XAXIS && vec2WSCoordinate.x >= 0)
		{
			rushdirection = glm::normalize(rushdirection);
			vec2WSCoordinate.x += rushdirection.x / cSettings->NUM_STEPS_PER_TILE_XAXIS;
			vec2WSCoordinate.y += rushdirection.y / cSettings->NUM_STEPS_PER_TILE_YAXIS;
			cout << rushdirection.x << "," << rushdirection.y << endl;
		}
		Constraint(LEFT);
		Constraint(RIGHT);
		Constraint(DOWN);
		Constraint(UP);
		/*vec2WSCoordinate.y = cSettings->ConvertIndexToWSSpace(cSettings->y, i32vec2Index.y, i32vec2NumMicroSteps.y);
		vec2WSCoordinate.x = cSettings->ConvertIndexToWSSpace(cSettings->x, i32vec2Index.x, i32vec2NumMicroSteps.x);*/

		if (CheckPosition(LEFT) == false)
		{
			vec2WSCoordinate.x += rushdirection.x / cSettings->NUM_STEPS_PER_TILE_XAXIS;
			cSettings->ConvertFloatToIndexSpace(cSettings->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
			rushdirection.x *= -1;
			wallcounter++;
		}
		if (CheckPosition(RIGHT) == false)
		{
			i32vec2NumMicroSteps.x = 0;
			vec2WSCoordinate.x = cSettings->ConvertIndexToWSSpace(cSettings->x, i32vec2Index.x, i32vec2NumMicroSteps.x);
			rushdirection.x *= -1;
			wallcounter++;
		}
		if (CheckPosition(UP) == false)
		{
			i32vec2NumMicroSteps.y = 0;
			vec2WSCoordinate.y = cSettings->ConvertIndexToWSSpace(cSettings->y, i32vec2Index.y, i32vec2NumMicroSteps.y);
			rushdirection.y *= -1;
			wallcounter++;
		}
		if (CheckPosition(DOWN) == false)
		{
			i32vec2Index.y++;
			i32vec2NumMicroSteps.y = 0;
			vec2WSCoordinate.y = cSettings->ConvertIndexToWSSpace(cSettings->y, i32vec2Index.y, i32vec2NumMicroSteps.y);
			rushdirection.y *= -1;
			wallcounter++;
		}
		if (CPlayer2D::GetInstance()->vec2WSCoordinate.x < vec2WSCoordinate.x)
		{
			animatedSprites->PlayAnimation("left", -1, 1.0f);
		}
		else if (CPlayer2D::GetInstance()->vec2WSCoordinate.x > vec2WSCoordinate.x)
		{
			animatedSprites->PlayAnimation("right", -1, 1.0f);
		}
		else
		{
			animatedSprites->PlayAnimation("idle", -1, 1.0f);
		}
		cout << "RUSHATTACK" << endl;
		break;
	default:
		break;
	}
	// Update Jump or Fall
	// UpdateJumpFall(dElapsedTime);
	animatedSprites->Update(dElapsedTime);
}
