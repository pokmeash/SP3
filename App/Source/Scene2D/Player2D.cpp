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
#include "EventControl/EventHandler.h"
#include "EventControl/Player2DMoveEvent.h"
#include "EventControl/Item2DPickUpEvent.h"
#include "EventControl/NextRoomEvent.h"
#include "Scene2D.h"

#include "EntityFactory.h"
#include "EntityManager.h"
#include "Projectile/PortalManager.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::CPlayer2D(void)
	: cKeyboardController(NULL)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
	, cSoundController(NULL)
	, cMouseController(NULL)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	rotation = 0.f;
	scale = glm::vec3(1, 1, 1);
	// Initialise vecIndex
	i32vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	i32vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);
	//Attributes

	/*setHP(3);
	setMaxHP(4);
	setProjSpeed(0.5);
	setDmg(1);*/
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

	PortalManager::GetInstance()->Reset();

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
	srand(static_cast <unsigned> (time(0)));
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();
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
	/*if (LoadTexture("Image/scene2d_player.png", iTextureID) == false)
	{
		std::cout << "Failed to load player tile texture" << std::endl;
		return false;
	}*/
	if (LoadTexture("Image/test.png", iTextureID) == false)
	{
		std::cout << "Failed to load player tile texture" << std::endl;
		return false;
	}
	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(4, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idle", 0, 3);
	animatedSprites->AddAnimation("up", 4, 7);
	animatedSprites->AddAnimation("right", 12, 15);
	animatedSprites->AddAnimation("left", 8, 11);

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
	cInventoryItem = cInventoryManager->Add("Lives", "Image/health.tga", getHP(), 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Damage", "Image/attackup.tga", getDmg(), 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Health icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Health", "Image/Scene2D_Health.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Wing icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Wing", "Image/Scene2D_WingsTile.tga", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	jumpCount = 0;

	dirx = 0;
	diry = -1;

	// Get the handler to the CSoundController
	cSoundController = CSoundController::GetInstance();
	swap = true;
	bIsActive = true;

	PortalManager::GetInstance()->Init();
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

	} else if (cKeyboardController->IsKeyDown(cSettings->iKeybinds[CSettings::MOVE_RIGHT]))
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
		animatedSprites->PlayAnimation("up", -1, 1.0f);
		currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	} else if (cKeyboardController->IsKeyDown(cSettings->iKeybinds[CSettings::MOVE_DOWN]))
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


	}
	vec2Velocity = vec2WSCoordinate - vec2WSOldCoordinates;
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
		cSoundController->PlaySoundByID(CSoundController::SOUNDS::SWAP);


	} else if ((cMouseController->IsButtonPressed(cSettings->iKeybinds[CSettings::TRIGGER_POWERUP]) || cKeyboardController->IsKeyPressed(cSettings->iKeybinds[CSettings::TRIGGER_POWERUP])) && swap == false)
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
		cSoundController->PlaySoundByID(CSoundController::SOUNDS::SWAP);
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
		if (cInventoryManager->Check("Tree"))
		{
			cInventoryItem = cInventoryManager->GetItem("Tree");
			for (int i = -cInventoryItem->GetCount(); i <= cInventoryItem->GetCount(); i++)
			{
				glm::vec2 temp = direction;
				temp.y = sinf(atan2f(temp.y, temp.x) + 0.1 * i);
				temp.x = cosf(atan2f(temp.y, temp.x) + 0.1 * i);
				temp = glm::normalize(temp) * getProjSpeed();  // projectile speed
				EntityFactory::GetInstance()->ProduceBullets(vec2WSCoordinate, glm::vec2(temp.x, temp.y), glm::vec3(1, 1, 1), E_BULLET)->setDmg(getDmg());
			}
		}
	}
	if (cKeyboardController->IsKeyDown(cSettings->iKeybinds[CSettings::TRIGGER_THROW]) && delay <= 0.f) // Throwing of grenade
	{
		delay = 0.5f;
		glm::i32vec2 mouse((int)CMouseController::GetInstance()->GetMousePositionX(), (int)CMouseController::GetInstance()->GetMousePositionY());
		glm::vec2 wsSpace(0.f, 0.f);
		cSettings->ConvertMouseToWSSpace(mouse.x, mouse.y, &(wsSpace.x), &(wsSpace.y));
		glm::vec2 direction = wsSpace - vec2WSCoordinate;
		direction = glm::normalize(direction);
		EntityFactory::GetInstance()->ProduceGrenade(vec2WSCoordinate, direction, glm::vec3(1, 1, 1), E_GRENADE);
		//EntityFactory::GetInstance()->ProduceBeam(vec2WSCoordinate, direction, E_BEAM, 50);
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_B) && delay <= 0.f) // Throwing of grenade
	{
		delay = 0.5f;
		glm::i32vec2 mouse((int)CMouseController::GetInstance()->GetMousePositionX(), (int)CMouseController::GetInstance()->GetMousePositionY());
		glm::vec2 wsSpace(0.f, 0.f);
		cSettings->ConvertMouseToWSSpace(mouse.x, mouse.y, &(wsSpace.x), &(wsSpace.y));
		glm::vec2 direction = wsSpace - vec2WSCoordinate;
		direction = glm::normalize(direction);
		//EntityFactory::GetInstance()->ProduceGrenade(vec2WSCoordinate, direction, glm::vec3(1, 1, 1), E_GRENADE);
		EntityFactory::GetInstance()->ProduceBeam(vec2WSCoordinate, direction, E_BEAM, 50);
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
	if (vec2WSOldCoordinates != vec2WSCoordinate) {
		if (EventHandler::GetInstance()->CallDeleteIsCancelled(new Player2DMoveEvent(this, vec2WSCoordinate, vec2WSOldCoordinates))) {
			vec2WSCoordinate = vec2WSOldCoordinates;
		}
	}

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertFloatToUVSpace(cSettings->x, vec2WSCoordinate.x, false);
	vec2UVCoordinate.y = cSettings->ConvertFloatToUVSpace(cSettings->y, vec2WSCoordinate.y, false);

	if (iframesState == true)
	{
		iframesDuration -= dElapsedTime;
		iframesTimer -= dElapsedTime;

		if (iframesTimer < 0.1 && iFrames == false)
		{
			iFrames = true;
		}

		else if (iframesTimer <= 0 && iFrames == true)
		{
			iFrames = false;
			iframesTimer = 0.2;
		}

		if (iframesDuration <= 0)
		{
			iframesState = false;
			iFrames = false;
			iframesDuration = 3;
		}

	}
	if (vec2WSOldCoordinates != vec2WSCoordinate) {
		EventHandler::GetInstance()->CallThenDelete(new Player2DMoveEvent(this, vec2WSCoordinate, vec2WSOldCoordinates));
	}
	CInventoryItem* portalItem = cInventoryManager->GetItem("Portal");
	if (!cInventoryManager->Check("Portal")) {
		PortalManager::GetInstance()->Update(dElapsedTime);
	}
	if (cMap2D->GetMapInfo(22,16) == 100 && cMap2D->once == false)
	{
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
				for (int i = 0; i < 4; i++)
				{
					unsigned int DoorRow = cSettings->NUM_TILES_YAXIS;
					unsigned int DoorCol = cSettings->NUM_TILES_XAXIS;
					if (cMap2D->FindValue(100, DoorRow, DoorCol) == false)
						return;

					if (cMap2D->GetDoorInfo(DoorRow, DoorCol) != -1)
					{
						cMap2D->SetMapInfo(DoorRow, DoorCol, 97);
					}
					else
					{
						cMap2D->SetMapInfo(DoorRow, DoorCol, 99);
					}

				}
				cSoundController->PlaySoundByID(CSoundController::SOUNDS::DOOR);
				cMap2D->once = true;
			}
		}
		if (CScene2D::GetInstance()->enemyVector.size() == 0)
		{
			for (int i = 0; i < 4; i++)
			{
				unsigned int DoorRow = cSettings->NUM_TILES_YAXIS;
				unsigned int DoorCol = cSettings->NUM_TILES_XAXIS;
				if (cMap2D->FindValue(100, DoorRow, DoorCol) == false)
					return;
				if (cMap2D->GetDoorInfo(DoorRow, DoorCol) != -1)
				{
					cMap2D->SetMapInfo(DoorRow, DoorCol, 97);
				}
				else
				{
					cMap2D->SetMapInfo(DoorRow, DoorCol, 99);
				}
			
				cMap2D->once = true;
			}
		}
	}
	else if (cMap2D->GetMapInfo(22, 16) == 101 && cMap2D->once == false)
	{
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
				cMap2D->SetMapInfo(12,16, 98);// next floor
				cMap2D->SetMapInfo(8, 16, 2); // powerup
				cSoundController->PlaySoundByID(CSoundController::SOUNDS::DOOR);
				cMap2D->once = true;
			}
		}
	}


}

void CPlayer2D::Render(void)
{
	if (iFrames == false)
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
	PortalManager::GetInstance()->Render();
}

void CPlayer2D::PlayerDamaged(int amtOfDmg)
{
	
	if (iframesState == false)
	{
		minusHP(amtOfDmg);
		CGameManager::GetInstance()->addDamageTaken(amtOfDmg);
		currentColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
		iframesState = true;
	}

	else if (iframesState == true)
	{
		currentColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
	}

}

bool CPlayer2D::getiFramesState()
{
	return iframesState;
}


void CPlayer2D::addRicochetTimes(int times)
{
	numberOfRicochet += times;
}

int CPlayer2D::getRicochetTimes()
{
	return numberOfRicochet;
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
		EventHandler::GetInstance()->CallThenDelete(new Item2DPickUpEvent("Tree", cInventoryItem, i32vec2Index));
		break;
	case 4:
		cSoundController->PlaySoundByID(5);
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		break;
	case 5:
		cInventoryItem = cInventoryManager->GetItem("Portal");
		cInventoryItem->Add(1);
		EventHandler::GetInstance()->CallThenDelete(new Item2DPickUpEvent("Portal", cInventoryItem, i32vec2Index));
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		break;
	case 10:
		// Increase the lives by 1
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Add(1);
		// Erase the life from this position
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		break;
	case 11:
		addProjSpeed(0.1);
		CGameManager::GetInstance()->addPowerUp(1);
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		break;
	case 12:
		addDmg(1);
		CGameManager::GetInstance()->addPowerUp(1);
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		break;
	case 13:
		addMaxHP(1);
		CGameManager::GetInstance()->addPowerUp(1);
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		break;
	case 14:
		if (getHP() != getMaxHP())
		{
			addHP(1);
		}
		CGameManager::GetInstance()->addPowerUp(1);
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		break;
	case 15:
		addRicochetTimes(1);
		CGameManager::GetInstance()->addPowerUp(1);
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
	case 97:
		//Next Room
		EventHandler::GetInstance()->CallThenDelete(new NextRoomEvent());
		if (i32vec2Index.x == 16 && i32vec2Index.y == 22)
		{
			//Top
			CScene2D::GetInstance()->LevelCompleted(0);
		}
		else if (i32vec2Index.x == 30 && i32vec2Index.y == 11)
		{
			//Right
			CScene2D::GetInstance()->LevelCompleted(1);
		}
		else if (i32vec2Index.x == 16 && i32vec2Index.y == 1)
		{
			//Bot
			CScene2D::GetInstance()->LevelCompleted(2);
		}
		else if (i32vec2Index.x == 1 && i32vec2Index.y == 11)
		{
			//Left
			CScene2D::GetInstance()->LevelCompleted(3);
		}
		break;
	case 98:
		//Next Room
		//next level
		EventHandler::GetInstance()->CallThenDelete(new NextRoomEvent());
		CScene2D::GetInstance()->LevelCompleted(4);
		break;
	case 99:
		//Next Room
		EventHandler::GetInstance()->CallThenDelete(new NextRoomEvent());
		if (i32vec2Index.x == 16 && i32vec2Index.y == 22)
		{
			//Top
			CScene2D::GetInstance()->LevelCompleted(0);
		}
		else if (i32vec2Index.x == 30 && i32vec2Index.y == 11)
		{
			//Right
			CScene2D::GetInstance()->LevelCompleted(1);
		}
		else if (i32vec2Index.x == 16 && i32vec2Index.y == 1)
		{
			//Bot
			CScene2D::GetInstance()->LevelCompleted(2);
		}
		else if (i32vec2Index.x == 1 && i32vec2Index.y == 11)
		{
			//Left
			CScene2D::GetInstance()->LevelCompleted(3);
		}
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
