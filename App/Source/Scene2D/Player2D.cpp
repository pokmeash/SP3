/**
 Player2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Player2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "FloorManager.h"
#include "Primitives/MeshBuilder.h"
#include "Inputs/MouseController.h"
#include "System/MyMath.h"
// Include Game Manager
#include "GameManager.h"

#include "Scene2D.h"


/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::CPlayer2D(void)
	: cKeyboardController(NULL)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
	, cSoundController(NULL)
	, cEntityManager(NULL)
	, cEntityFactory(NULL)
	, cMouseController(NULL)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	i32vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	i32vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::~CPlayer2D(void)
{
	// We won't delete this since it was created elsewhere
	cSoundController = NULL;

	// We won't delete this since it was created elsewhere
	cInventoryManager = NULL;

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	cMouseController = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	cEntityManager = NULL;
	cEntityFactory = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

/**
  @brief Initialise this instance
  */
bool CPlayer2D::Init(void)
{
	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	cMouseController = CMouseController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();
	cEntityManager = EntityManager::GetInstance();
	// Get the handler to the CMap2D instance
	cMap2D = CFloorManager::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(3, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	vec2WSCoordinate = i32vec2Index;
	vec2WSCoordinate.x += 0.5;
	vec2Velocity = glm::vec2(0.f, 0.f);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	// Load the player texture
	if (LoadTexture("Image/scene2d_player.png", iTextureID) == false)
	{
		std::cout << "Failed to load player tile texture" << std::endl;
		return false;
	}
	
	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(6, 3, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idle", 0, 2);
	animatedSprites->AddAnimation("right", 3, 5);
	animatedSprites->AddAnimation("left", 6, 8);
	animatedSprites->AddAnimation("idleWing", 9, 11);
	animatedSprites->AddAnimation("rightWing", 12, 14);
	animatedSprites->AddAnimation("leftWing", 15, 17);
	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	// Get the handler to the CInventoryManager instance
	cInventoryManager = CInventoryManager::GetInstance();
	// Add a Lives icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Lives", "Image/Scene2D_Lives.tga", 3, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Health icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Health", "Image/Scene2D_Health.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Wing icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Wing", "Image/Scene2D_WingsTile.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);
	
	// Add a Wing icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("DoubleShot", "Image/DoubleShot.png", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	jumpCount = 0;

	dirx = 0;
	diry = -1;

	// Get the handler to the CSoundController
	cSoundController = CSoundController::GetInstance();
	swap = true;
	bIsActive = true;
	return true;
}

/**
 @brief Reset this instance
 */
bool CPlayer2D::Reset()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(3, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	vec2WSCoordinate = i32vec2Index;
	vec2WSCoordinate.x += 0.5;
	vec2Velocity = glm::vec2(0.f, 0.f);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	//Set it to fall upon entering new level
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	//CS: Reset double jump
	jumpCount = 0;

	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	dirx = 0;
	diry = 1;

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	swap = true;

	return true;
}

/**
 @brief Update this instance
 */
void CPlayer2D::Update(const double dElapsedTime)
{
	// Store the old position
	vec2WSOldCoordinates = vec2WSCoordinate;

	// Get keyboard updates
	if (cKeyboardController->IsKeyDown(cSettings->iKeybinds[CSettings::MOVE_LEFT]))
	{
		// Calculate the new position to the left
		if (vec2WSCoordinate.x >= 0)
		{
			vec2WSCoordinate.x -= 4.f / cSettings->NUM_STEPS_PER_TILE_XAXIS;
		}
		cSettings->ConvertFloatToIndexSpace(cSettings->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
		
		// Constraint the player's position within the screen boundary
		Constraint(LEFT);

		// If the new position is not feasible, then revert to old position
		if (CheckPosition(LEFT) == false)
		{
			vec2WSCoordinate.x += 4.f / cSettings->NUM_STEPS_PER_TILE_XAXIS;
			cSettings->ConvertFloatToIndexSpace(cSettings->x, vec2WSCoordinate.x, &i32vec2Index.x, &i32vec2NumMicroSteps.x);
		}

		// Check if player is in mid-air, such as walking off a platform
		/*if (IsMidAir() == true)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}*/

		//CS: Play the "left" animation
		animatedSprites->PlayAnimation("left", -1, 1.0f);
		currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

		dirx = -1;
		diry = 0;
	}
	else if (cKeyboardController->IsKeyDown(cSettings->iKeybinds[CSettings::MOVE_RIGHT]))
	{
		// Calculate the new position to the right
		if (vec2WSCoordinate.x < cSettings->NUM_TILES_XAXIS)
		{
			vec2WSCoordinate.x += 4.f / cSettings->NUM_STEPS_PER_TILE_XAXIS;
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
		}

		// Check if player is in mid-air, such as walking off a platform
		/*if (IsMidAir() == true)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}*/

		//CS: Play the "right" animation
		animatedSprites->PlayAnimation("right", -1, 1.0f);
		currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

		dirx = 1;
		diry = 0;
	}
	if (cKeyboardController->IsKeyDown(cSettings->iKeybinds[CSettings::MOVE_UP]))
	{
		// Calculate the new position up
		if (vec2WSCoordinate.y < cSettings->NUM_TILES_YAXIS)
		{
			vec2WSCoordinate.y += 4.f / cSettings->NUM_STEPS_PER_TILE_YAXIS;
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

		//CS: Play the "idle" animation
		animatedSprites->PlayAnimation("idle", -1, 1.0f);
		currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

		dirx = 0;
		diry = 1;
	}
	else if (cKeyboardController->IsKeyDown(cSettings->iKeybinds[CSettings::MOVE_DOWN]))
	{
		// Calculate the new position down
		if (vec2WSCoordinate.y >= 0)
		{
			vec2WSCoordinate.y -= 4.f / cSettings->NUM_STEPS_PER_TILE_YAXIS;
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
		//CS: Play the "idle" animation
		animatedSprites->PlayAnimation("idle", -1, 1.0f);
		currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

		dirx = 0;
		diry = -1;
	}
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_DOWN))
	{
		CGameManager::GetInstance()->bLevelCompleted = true;
	}
	//Swapping
	bool activate = cSettings->iKeybinds[CSettings::TRIGGER_POWERUP] <= GLFW_MOUSE_BUTTON_LAST && cMouseController->IsButtonPressed(cSettings->iKeybinds[CSettings::TRIGGER_POWERUP]);
	if (activate || cKeyboardController->IsKeyPressed(cSettings->iKeybinds[CSettings::TRIGGER_POWERUP]) && swap == true)
	{
		unsigned int InvisRow = -1;
		unsigned int InvisCol = -1;
		unsigned int InvisRow2 = -1;
		unsigned int InvisCol2 = -1;
		bool counter = true;
		bool counter2 = true;
		swap = false;
		cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

		while (counter)
		{
			counter = cMap2D->FindValue(97, InvisRow, InvisCol);
			if (cMap2D->FindValue(97, InvisRow, InvisCol) == false)
			{
			}
			if (!counter) break;
			cMap2D->SetMapInfo(InvisRow, InvisCol, 101);
		}
		while (counter2)
		{
			counter2 = cMap2D->FindValue(102, InvisRow, InvisCol);
			if (cMap2D->FindValue(102, InvisRow, InvisCol) == false)
			{
			}
			if (!counter2) break;
			cMap2D->SetMapInfo(InvisRow, InvisCol, 96);
		}
		cSoundController->PlaySoundByID(8);


	}
	else if ((cMouseController->IsButtonPressed(cSettings->iKeybinds[CSettings::TRIGGER_POWERUP]) ||cKeyboardController->IsKeyPressed(cSettings->iKeybinds[CSettings::TRIGGER_POWERUP])) && swap == false)
	{
		unsigned int InvisRow = -1;
		unsigned int InvisCol = -1;
		unsigned int InvisRow2 = -1;
		unsigned int InvisCol2 = -1;
		bool counter = true;
		bool counter2 = true;
		swap = true;

		cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		while (counter)
		{
			counter = cMap2D->FindValue(101, InvisRow, InvisCol);
			if (cMap2D->FindValue(101, InvisRow, InvisCol) == false)
			{
			}
			if (!counter) break;
			cMap2D->SetMapInfo(InvisRow, InvisCol, 97);
		}
		while (counter2)
		{
			counter2 = cMap2D->FindValue(96, InvisRow2, InvisCol2);
			if (cMap2D->FindValue(96, InvisRow2, InvisCol2) == false)
			{
			}
			if (!counter2) break;
			cMap2D->SetMapInfo(InvisRow2, InvisCol2, 102);
		}
		cSoundController->PlaySoundByID(8);
	}

	static float delay = 0.f;
	bool shoot = cSettings->iKeybinds[CSettings::TRIGGER_SHOOT] <= GLFW_MOUSE_BUTTON_LAST && cMouseController->IsButtonDown(cSettings->iKeybinds[CSettings::TRIGGER_SHOOT]);
	if ((shoot || cKeyboardController->IsKeyDown(cSettings->iKeybinds[CSettings::TRIGGER_SHOOT])) && delay <= 0.f)
	{
		delay = 0.5f;
		glm::i32vec2 mouse((int)CMouseController::GetInstance()->GetMousePositionX(), (int)CMouseController::GetInstance()->GetMousePositionY());
		glm::vec2 wsSpace(0.f, 0.f);
		cSettings->ConvertMouseToWSSpace(mouse.x, mouse.y, &(wsSpace.x), &(wsSpace.y));
		glm::vec2 direction = wsSpace - vec2WSCoordinate;
		direction = glm::normalize(direction);
		direction *= 0.5f;
		if (cInventoryManager->Check("Tree"))
		{
			cInventoryItem = cInventoryManager->GetItem("Tree");
			for (int i = -cInventoryItem->GetCount(); i <= cInventoryItem->GetCount(); i++)
			{
				glm::vec2 temp = direction;
				temp.y = sinf(atan2f(temp.y, temp.x) + 0.1 * i);
				temp.x = cosf(atan2f(temp.y, temp.x) + 0.1 * i);
				temp = glm::normalize(temp) * 0.5f;
				cEntityManager->entitylist.push_back(cEntityFactory->ProduceBullets(vec2WSCoordinate, glm::vec2(temp.x, temp.y), glm::vec3(1, 1, 1), 0, E_BULLET));
			}
		}
	}
	if (delay > 0) {
		delay -= dElapsedTime;
	}

	// Interact with the Map
	InteractWithMap();

	// Update the Health and Lives
	UpdateHealthLives();

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertFloatToUVSpace(cSettings->x, vec2WSCoordinate.x, false);
	vec2UVCoordinate.y = cSettings->ConvertFloatToUVSpace(cSettings->y, vec2WSCoordinate.y, false);
	int counter = 0;
	for (std::vector<CEntity2D*>::iterator it2 = CScene2D::GetInstance()->enemyVector.begin(); it2 != CScene2D::GetInstance()->enemyVector.end(); ++it2)
	{
		CEntity2D* enemy = (CEntity2D*)*it2;
		if (enemy->bIsActive == false)
		{
			counter++;
		}
		if (CScene2D::GetInstance()->enemyVector.size() == counter)
		{
			unsigned int DoorRow = -1;
			unsigned int DoorCol = -1;
			if (cMap2D->FindValue(100, DoorRow, DoorCol) == false)
				return;
			cMap2D->SetMapInfo(DoorRow, DoorCol, 99);
			cSoundController->PlaySoundByID(6);
		}
	}

}

void CPlayer2D::Render(void)
{
	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtime_color");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x,
		vec2UVCoordinate.y,
		0.0f));
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(currentColor));
	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	//CS: Render the animated sprite
	animatedSprites->Render();
	glBindVertexArray(0);
}

void CPlayer2D::PlayerDamaged()
{
	cInventoryItem = cInventoryManager->GetItem("Health");
	cInventoryItem->Remove(20);
	currentColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
}

/**
 @brief Let player interact with the map. You can add collectibles such as powerups and health here.
 */
void CPlayer2D::InteractWithMap(void)
{
	switch (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x))
	{
	case 2:
		cSoundController->PlaySoundByID(5);
		// Erase the tree from this position
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		// Increase the Tree by 1
		cInventoryItem = cInventoryManager->GetItem("Tree");
		cInventoryItem->Add(1);

		break;
	case 4:
		cSoundController->PlaySoundByID(5);
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		wings = true;
		break;
	case 10:
		// Increase the lives by 1
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Add(1);
		// Erase the life from this position
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		break;
	case 20:
		// Decrease the health by 1
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Remove(1);
		currentColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
		break;
	case 21:
		// Increase the health
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Add(1);
		currentColor = glm::vec4(0.0, 1.0, 0.0, 1.0);
		break;
	case 99:
		//Next Room
		
		CGameManager::GetInstance()->bLevelCompleted = true;
		break;
	default:
		break;
	}
}

/**
 @brief Update the health and lives.
 */
void CPlayer2D::UpdateHealthLives(void)
{
	// Update health and lives
	cInventoryItem = cInventoryManager->GetItem("Health");
	// Check if a life is lost
	if (cInventoryItem->GetCount() <= 0)
	{
		// Reset the Health to max value
		cInventoryItem->iItemCount = cInventoryItem->GetMaxCount();
		// But we reduce the lives by 1.
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Add(1);
		i32vec2Index.x = 1;
		i32vec2Index.y = 5;
		currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
		// Check if there is no lives left...
		//if (cInventoryItem->GetCount() <= 0)
		//{
		//	// Player loses the game
		//	CGameManager::GetInstance()->bPlayerLost = true;
		//}
	}
}
