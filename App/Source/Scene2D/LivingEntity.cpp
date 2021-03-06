/**
 CLivingEntity
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "LivingEntity.h"

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
#include "FloorManager.h"
// Include math.h
#include <math.h>

#include "Player2D.h"

#include "EntityManager.h"

#include "EventControl/EventHandler.h"
#include "EventControl/Entity2DMoveEvent.h"
/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CLivingEntity::CLivingEntity(void)
	: cMap2D(NULL)
	, sCurrentFSM(FSM::IDLE)
	, iFSMCounter(0)
	, BaseAttribute(1,1,1)
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
CLivingEntity::~CLivingEntity(void)
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
@brief Set the indices of the enemy2D
@param iIndex_XAxis A const int variable which stores the index in the x-axis
@param iIndex_YAxis A const int variable which stores the index in the y-axis
*/
void CLivingEntity::Seti32vec2Index(const int iIndex_XAxis, const int iIndex_YAxis)
{
	this->i32vec2Index.x = iIndex_XAxis;
	this->i32vec2Index.y = iIndex_YAxis;
	vec2WSCoordinate.x = cSettings->ConvertIndexToWSSpace(cSettings->x, i32vec2Index.x, i32vec2NumMicroSteps.x);
	vec2WSCoordinate.y = cSettings->ConvertIndexToWSSpace(cSettings->y, i32vec2Index.y, i32vec2NumMicroSteps.y);
}

/**
@brief Set the number of microsteps of the enemy2D
@param iNumMicroSteps_XAxis A const int variable storing the current microsteps in the X-axis
@param iNumMicroSteps_YAxis A const int variable storing the current microsteps in the Y-axis
*/
void CLivingEntity::Seti32vec2NumMicroSteps(const int iNumMicroSteps_XAxis, const int iNumMicroSteps_YAxis)
{
	this->i32vec2NumMicroSteps.x = iNumMicroSteps_XAxis;
	this->i32vec2NumMicroSteps.y = iNumMicroSteps_YAxis;
	vec2WSCoordinate.x = cSettings->ConvertIndexToWSSpace(cSettings->x, i32vec2Index.x, i32vec2NumMicroSteps.x);
	vec2WSCoordinate.y = cSettings->ConvertIndexToWSSpace(cSettings->y, i32vec2Index.y, i32vec2NumMicroSteps.y);
}

/**
 @brief Constraint the enemy2D's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CLivingEntity::Constraint(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if (i32vec2Index.x < 0)
		{
			i32vec2Index.x = 0;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == RIGHT)
	{
		if (i32vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			i32vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS) - 1;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == UP)
	{
		if (i32vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			i32vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS) - 1;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else if (eDirection == DOWN)
	{
		if (i32vec2Index.y < 0)
		{
			i32vec2Index.y = 0;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else
	{
		cout << "CLivingEntity::Constraint: Unknown direction." << endl;
	}
}

/**
 @brief Check if a position is possible to move into
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
bool CLivingEntity::CheckPosition(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		// If the new position is fully within a row, then check this row only
		if (i32vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (i32vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == RIGHT)
	{
		// If the new position is at the top row, then return true
		if (i32vec2Index.x >= cSettings->NUM_TILES_XAXIS - 1)
		{
			i32vec2NumMicroSteps.x = 0;
			return true;
		}

		// If the new position is fully within a row, then check this row only
		if (i32vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (i32vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) >= 100) ||
				(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}

	}
	else if (eDirection == UP)
	{
		// If the new position is at the top row, then return true
		if (i32vec2Index.y >= cSettings->NUM_TILES_YAXIS - 1)
		{
			i32vec2NumMicroSteps.y = 0;
			return true;
		}

		// If the new position is fully within a column, then check this column only
		if (i32vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (i32vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == DOWN)
	{
		// If the new position is fully within a column, then check this column only
		if (i32vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (i32vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else
	{
		cout << "CLivingEntity::CheckPosition: Unknown direction." << endl;
	}
	return true;
}

void CLivingEntity::UpdateDirection(void)
{
	i32vec2Destination = CPlayer2D::GetInstance()->i32vec2Index;
	i32vec2Direction = i32vec2Destination - i32vec2Index;
	float fDistance = cPhysics2D.CalculateDistance(i32vec2Index, i32vec2Destination);
	if (fDistance >= 0.01f)
	{
		i32vec2Direction.x = (int)round(i32vec2Direction.x / fDistance);
		i32vec2Direction.y = (int)round(i32vec2Direction.y / fDistance);
	}
	else
	{
		i32vec2Direction = glm::i32vec2(0);
	}
}

void CLivingEntity::FlipHorizontalDirection(void)
{
	i32vec2Direction.x *= -1;
}

void CLivingEntity::UpdatePosition(void)
{
	vec2WSOldCoordinates = vec2WSCoordinate;
	if (i32vec2Direction.x < 0)
	{
		if (vec2WSCoordinate.x >= 0)
		{
			vec2WSCoordinate.x -= 1.f / cSettings->NUM_STEPS_PER_TILE_XAXIS;
		}
		cSettings->ConvertFloatToIndexSpace(cSettings->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
		Constraint(LEFT);
		vec2WSCoordinate.x = cSettings->ConvertIndexToWSSpace(cSettings->x, i32vec2Index.x, i32vec2NumMicroSteps.x);
		if (CheckPosition(LEFT) == false)
		{
			FlipHorizontalDirection();
			vec2WSCoordinate.x += 1.f / cSettings->NUM_STEPS_PER_TILE_XAXIS;
		}
		cSettings->ConvertFloatToIndexSpace(cSettings->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
		InteractWithPlayer();
	}
	else if (i32vec2Direction.x > 0)
	{
		if (vec2WSCoordinate.x < cSettings->NUM_TILES_XAXIS)
		{
			vec2WSCoordinate.x += 1.f / cSettings->NUM_STEPS_PER_TILE_XAXIS;
		}
		cSettings->ConvertFloatToIndexSpace(cSettings->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
		Constraint(RIGHT);
		vec2WSCoordinate.x = cSettings->ConvertIndexToWSSpace(cSettings->x, i32vec2Index.x, i32vec2NumMicroSteps.x);
		if (CheckPosition(RIGHT) == false)
		{
			FlipHorizontalDirection();
			i32vec2NumMicroSteps.x = 0;
			vec2WSCoordinate.x = cSettings->ConvertIndexToWSSpace(cSettings->x, i32vec2Index.x, i32vec2NumMicroSteps.x);
		}
		InteractWithPlayer();
	}
	if (i32vec2Direction.y > 0)
	{
		// Calculate the new position up
		if (vec2WSCoordinate.y < cSettings->NUM_TILES_YAXIS)
		{
			vec2WSCoordinate.y += 1.f / cSettings->NUM_STEPS_PER_TILE_YAXIS;
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
		}
	}
	else if (i32vec2Direction.y < 0)
	{
		// Calculate the new position down
		if (vec2WSCoordinate.y >= 0)
		{
			vec2WSCoordinate.y -= 1.f / cSettings->NUM_STEPS_PER_TILE_YAXIS;
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
		}
	}
	vec2Velocity = vec2WSCoordinate - vec2WSOldCoordinates;
	if (vec2WSCoordinate != vec2WSOldCoordinates) {
		if (EventHandler::GetInstance()->CallDeleteIsCancelled(new Entity2DMoveEvent(this, vec2WSCoordinate, vec2WSOldCoordinates))) {
			vec2WSCoordinate = vec2WSOldCoordinates;
		}
	}
	CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
	CSettings::GetInstance()->ConvertFloatToIndexSpace(CSettings::GetInstance()->y, vec2WSCoordinate.y, &i32vec2Index.y, &i32vec2NumMicroSteps.y);
}

void CLivingEntity::PathFinding(void)
{
	auto path = cMap2D->PathFind(i32vec2Index,
		CPlayer2D::GetInstance()->i32vec2Index,
		heuristic::euclidean,
		10);
	bool bFirstPosition = true;
	for (const auto& coord : path)
	{
		if (bFirstPosition == true)
		{
			// Set a destination
			i32vec2Destination = coord;
			// Calculate the direction between enemy2D and this destination
			i32vec2Direction = i32vec2Destination - i32vec2Index;
			bFirstPosition = false;
		} else
		{
			if ((coord - i32vec2Destination) == i32vec2Direction)
			{
				i32vec2Destination = coord;
			} else
				break;
		}
	}
}
